<?php
/**
 * crc_fun.php -- predict OHOL curse names from email addresses.
 *
 * Requires PHP 7+ (64-bit). No external dependencies.
 *
 * Usage:
 *   require 'crc_fun.php';
 *   echo get_curse_by_email('someone@example.com');  // e.g. "WHEAT_LAKE_TREE"
 */

// Calibration constants -- patch after running: crc-fun calibrate observations.txt
const _CRC_FUN_SECRET_LEN   = 21;
const _CRC_FUN_SECRET_CRC32 = 0x31426be9;

const _CRC_FUN_WORDS = [
    "WHEAT", "HOUND", "RIVER", "QUEEN", "BLOOM", "GEESE", "TREE",  "DOLL",
    "THORN", "HORSE", "LAKE",  "SPRAY", "GRASS", "BRUTE", "BOOK",  "CASH",
    "BOWL",  "IDEA",  "WOODS", "BOARD", "ELBOW", "MULE",  "LINK",  "AGONY",
    "SPIRE", "HALL",  "PRIDE", "HINT",  "STAR",  "MONK",  "APPLE", "CLAW",
    "TOWER", "HOUSE", "HIDE",  "SOUL",  "MOSS",  "SEAT",  "MONEY", "BOSS",
    "HOOF",  "BEAST", "STYLE", "STEAM", "OCEAN", "DAWN",  "COAST", "FLESH",
    "SNAKE", "GIRL",  "BIRD",  "FORM",  "DUTY",  "SWAMP", "CRIME", "FATE",
    "PUPIL", "FROG",  "PAPER", "OVEN",  "CHARM", "SHORE", "DRESS", "MOOD",
    "ROCK",  "SHOES", "LIFE",  "TRUTH", "COST",  "CIGAR", "TOOL",  "CABIN",
    "DEED",  "DEATH", "MONTH", "FOAM",  "BODY",  "QUEST", "STAIN", "WINE",
    "GIFT",  "POLE",  "TOAST", "FLOOD", "FACT",  "PANIC", "FOLLY", "JOKE",
    "CHILD", "LIMB",  "METAL", "PEACH", "SUGAR", "DOVE",  "GOLF",  "LIME",
    "DUST",  "SALAD", "GRIEF", "DOOR",  "CHAIR", "WORLD", "MERCY", "POET",
    "PIANO", "PLAIN", "FAULT", "PARTY", "SKULL", "ABODE", "HOTEL", "STORM",
    "PLANK", "ARMY",  "HOPE",  "STONE", "TABLE", "CODE",  "FORK",  "SHIP",
    "CORD",  "KING",  "FOWL",  "DELL",  "CAMP",  "MIND",  "WOMAN", "NAIL",
    "POWER", "HOME",  "SOIL",  "DEVIL", "ANGER", "EVENT", "LOVE",  "VEST",
    "WIFE",  "ARRAY", "ODOUR", "TANK",  "SHAME", "MORAL", "TIME",  "DRAMA",
    "SHOCK", "HARP",  "KISS",  "THIEF", "CHIEF", "ANKLE", "OATS",  "CANE",
    "PLANT", "LAWN",  "CORN",  "TRUCK", "CITY",  "DIRT",  "MAKER", "DREAM",
    "WATER", "TOMB",  "EARTH", "HOUR",  "ARROW", "BRAIN", "CHIN",  "BARON",
    "GLORY", "SLAVE", "JUDGE", "COIN",  "MAST",  "PIPE",  "OWNER", "IRON",
    "SKIN",  "GHOST", "JELLY", "NYMPH", "BLOOD", "CLOCK", "LORD",  "LARK",
    "JAIL",  "SAUCE", "UNIT",  "GOLD",  "LEMON", "BOSOM", "ITEM",  "JURY",
    "FIRE",  "CELL",  "ANGLE", "GREEN", "CANDY", "FLAG",  "LUMP",  "MEAT",
    "BABY",
];

// ---------------------------------------------------------------------------
// CRC32 combine
//
// CRC32 is linear over GF(2):
//   CRC32(A || B) = _crc_shift(CRC32(A), len(B)) ^ CRC32(B)
//
// _crc_shift advances a CRC state as if $n zero bytes were appended,
// via repeated GF(2) matrix squaring (same algorithm as zlib crc32_combine).
//
// hash('crc32b', ...) uses the same polynomial/init/finalization as the
// server (CRC-32/ISO-HDLC). unpack('N', ...) gives an unsigned 32-bit int
// portably across 32-bit and 64-bit PHP.
// ---------------------------------------------------------------------------

function _crc_s(string $s): int {
    return unpack('N', hash('crc32b', $s, true))[1];
}

function _gf2_mat_times(array $mat, int $vec): int {
    $result = 0;
    foreach ($mat as $row) {
        if ($vec & 1) $result ^= $row;
        $vec >>= 1;
        if (!$vec) break;
    }
    return $result;
}

function _gf2_mat_square(array $mat): array {
    return array_map(fn($row) => _gf2_mat_times($mat, $row), $mat);
}

function _crc_shift(int $crc, int $n): int {
    if ($n === 0) return $crc;

    $odd = [0xedb88320];
    for ($i = 0; $i < 31; $i++) $odd[] = 1 << $i;

    $even = _gf2_mat_square($odd);
    $odd  = _gf2_mat_square($even);

    while (true) {
        $even = _gf2_mat_square($odd);
        if ($n & 1) $crc = _gf2_mat_times($even, $crc);
        $n >>= 1;
        if ($n === 0) break;

        $odd = _gf2_mat_square($even);
        if ($n & 1) $crc = _gf2_mat_times($odd, $crc);
        $n >>= 1;
        if ($n === 0) break;
    }

    return $crc;
}

function _crc_combine(int $crc_a, int $crc_b, int $len_b): int {
    return _crc_shift($crc_a, $len_b) ^ $crc_b;
}

// ---------------------------------------------------------------------------
// Jenkins small PRNG
//
// Reproduces JenkinsRandomSource + RandomSource32::getRandomBoundedInt exactly.
// The key subtlety: the C++ original computes mInvMAXPlusOne as
//   1.0 / ((float)UINT32_MAX + 1.0)
// where the cast to C `float` (32-bit) rounds 4294967295 up to 4294967296,
// giving 1.0 / 4294967297.0. We hardcode that constant directly.
//
// All arithmetic is masked to 32 bits since PHP uses 64-bit integers.
// ---------------------------------------------------------------------------

function _jenkins_words(int $seed, int $count): array {
    $M = 0xFFFFFFFF;

    $rot = function(int $x, int $k) use ($M): int {
        return (($x << $k) | ($x >> (32 - $k))) & $M;
    };

    $a = 0xf1ea5eed;
    $b = $c = $d = $seed & $M;

    $nxt = function() use (&$a, &$b, &$c, &$d, $rot, $M): int {
        $e = ($a - $rot($b, 27)) & $M;
        $a = ($b ^ $rot($c, 17)) & $M;
        $b = ($c + $d) & $M;
        $c = ($d + $e) & $M;
        $d = ($e + $a) & $M;
        return $d;
    };

    for ($i = 0; $i < 20; $i++) $nxt();

    $words  = _CRC_FUN_WORDS;
    $n      = count($words);
    $inv    = 1.0 / 4294967297.0;
    $result = [];

    for ($i = 0; $i < $count; $i++) {
        $r        = $nxt() * $inv;
        $result[] = $words[(int)($r * $n)];
    }

    return $result;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

function get_curse_by_email(string $email): string {
    $prefix = $email . "_";
    $seed   = _crc_combine(_crc_s($prefix), _CRC_FUN_SECRET_CRC32, _CRC_FUN_SECRET_LEN);
    return implode("_", _jenkins_words($seed, 3));
}
