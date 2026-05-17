"""
crc_fun.py -- predict OHOL curse names from email addresses.

Requires Python 3. No external dependencies.

Usage:
    from crc_fun import get_curse_by_email
    print(get_curse_by_email("someone@example.com"))  # e.g. "WHEAT_LAKE_TREE"
"""

import zlib

# Calibration constants -- patch after running: crc-fun calibrate observations.txt
_SECRET_LEN   = 21
_SECRET_CRC32 = 0x31426be9

_WORDS = (
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
)

# ---------------------------------------------------------------------------
# CRC32 combine
#
# CRC32 is linear over GF(2):
#   CRC32(A || B) = _crc32_shift(CRC32(A), len(B)) ^ CRC32(B)
#
# _crc32_shift advances a CRC state as if `n` zero bytes were appended,
# via repeated GF(2) matrix squaring (same algorithm as zlib crc32_combine).
# ---------------------------------------------------------------------------

_CRC32_POLY = 0xedb88320


def _gf2_mat_times(mat, vec):
    result = 0
    for row in mat:
        if vec & 1:
            result ^= row
        vec >>= 1
        if not vec:
            break
    return result


def _gf2_mat_square(mat):
    return [_gf2_mat_times(mat, row) for row in mat]


def _crc32_shift(crc, n):
    if n == 0:
        return crc

    odd  = [_CRC32_POLY] + [1 << i for i in range(31)]
    even = _gf2_mat_square(odd)
    odd  = _gf2_mat_square(even)

    while True:
        even = _gf2_mat_square(odd)
        if n & 1:
            crc = _gf2_mat_times(even, crc)
        n >>= 1
        if n == 0:
            break

        odd = _gf2_mat_square(even)
        if n & 1:
            crc = _gf2_mat_times(odd, crc)
        n >>= 1
        if n == 0:
            break

    return crc


def _crc32_combine(crc_a, crc_b, len_b):
    return _crc32_shift(crc_a, len_b) ^ crc_b


def _crc32s(s):
    """CRC32 of a UTF-8 string, unsigned 32-bit."""
    return zlib.crc32(s.encode()) & 0xFFFFFFFF


# ---------------------------------------------------------------------------
# Jenkins small PRNG
#
# Reproduces JenkinsRandomSource + RandomSource32::getRandomBoundedInt exactly.
# The key subtlety: the C++ original computes mInvMAXPlusOne as
#   1.0 / ((float)UINT32_MAX + 1.0)
# where the cast to C `float` (32-bit) rounds 4294967295 up to 4294967296,
# giving 1.0 / 4294967297.0. We hardcode that constant directly.
# ---------------------------------------------------------------------------

_INV_MAX_PLUS_ONE = 1.0 / 4294967297.0


def _jenkins_words(seed, count):
    M = 0xFFFFFFFF

    def rot(x, k):
        return ((x << k) | (x >> (32 - k))) & M

    a = 0xf1ea5eed
    b = c = d = seed & M

    def nxt():
        nonlocal a, b, c, d
        e = (a - rot(b, 27)) & M
        a = (b ^ rot(c, 17)) & M
        b = (c + d) & M
        c = (d + e) & M
        d = (e + a) & M
        return d

    for _ in range(20):
        nxt()

    n = len(_WORDS)
    result = []
    for _ in range(count):
        r = nxt() * _INV_MAX_PLUS_ONE
        result.append(_WORDS[int(r * n)])
    return result


# ---------------------------------------------------------------------------
# Public API
# ---------------------------------------------------------------------------

def get_curse_by_email(email):
    """Return the curse name for the given email, e.g. 'WHEAT_LAKE_TREE'."""
    prefix = email + "_"
    seed = _crc32_combine(_crc32s(prefix), _SECRET_CRC32, _SECRET_LEN)
    return "_".join(_jenkins_words(seed, 3))
