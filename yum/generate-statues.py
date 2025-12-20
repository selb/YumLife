#!/usr/bin/env python3
"""
Generate gameSource/yumStatues.h with statue data for GPS coordinate resolution.

This script finds the sequence of adjacent statues with the longest run and generates
a C++ header file with the data needed to match ST responses and determine
global X coordinate.
"""

import sys
import os
import re
from dataclasses import dataclass
from typing import Optional


@dataclass
class StatueData:
    """Represents a statue entry from statueForceLoad.txt"""
    x: int
    y: int
    display_id: int
    name: str
    clothing_set: str  # Semicolon-separated: hat;tunic;front_shoe;back_shoe;bottom;backpack
    final_words: str

    @classmethod
    def from_line(cls, line: str) -> Optional['StatueData']:
        """Parse a line from statueForceLoad.txt into a StatueData object"""
        # Format: x,y,statue_time#display_id|age|name|hat|tunic|front_shoe|back_shoe|bottom|backpack|final_words

        # Split at the # separator
        if '#' not in line:
            return None

        coords_part, data_part = line.split('#', 1)

        # Parse coordinates (ignore statue_time)
        coords_match = re.match(r'^(-?\d+),(-?\d+),', coords_part)
        if not coords_match:
            return None

        x = int(coords_match.group(1))
        y = int(coords_match.group(2))

        # Parse data string
        # Format: display_id|age|name|hat|tunic|front_shoe|back_shoe|bottom|backpack|final_words
        parts = data_part.split('|')
        if len(parts) != 10:
            return None

        try:
            display_id = int(parts[0])
            # Skip age (parts[1])
            name = parts[2]
            # Combine clothing items into protocol-style semicolon-separated string
            clothing_set = ";".join(parts[3:9])
            final_words = parts[9]
        except (ValueError, IndexError):
            return None

        return cls(
            x=x, y=y,
            display_id=display_id, name=name,
            clothing_set=clothing_set,
            final_words=final_words
        )


def load_statues(filepath: str) -> list[StatueData]:
    """Load all statues from a statueForceLoad.txt file"""
    statues = []

    with open(filepath, 'r') as f:
        for line_num, line in enumerate(f, 1):
            line = line.strip()
            if not line:
                continue

            statue = StatueData.from_line(line)
            if statue is None:
                print(f"Warning: Failed to parse line {line_num}: {line[:50]}...")
                continue

            statues.append(statue)

    return statues


def escape_c_string(s: str) -> str:
    """Escape a string for use in C++ string literal"""
    # Replace backslashes first to avoid double-escaping
    s = s.replace('\\', '\\\\')
    s = s.replace('"', '\\"')
    s = s.replace('\n', '\\n')
    s = s.replace('\r', '\\r')
    s = s.replace('\t', '\\t')
    return s


def find_best_y_coordinate(statues: list[StatueData]) -> tuple[int, list[StatueData]]:
    """
    Find the y-coordinate with the longest sequence of adjacent statues.

    Returns:
        Tuple of (best_y, sequence_of_statues)
    """
    # Partition statues by y-coordinate
    statues_by_y = {}
    for statue in statues:
        if statue.y not in statues_by_y:
            statues_by_y[statue.y] = []
        statues_by_y[statue.y].append(statue)

    # For each y-coordinate, find the longest sequence of adjacent statues
    best_y = None
    best_sequence = []

    for y, statues_at_y in statues_by_y.items():
        # Sort by x coordinate
        statues_at_y.sort(key=lambda s: s.x)

        # Find longest sequence of consecutive x coordinates
        if not statues_at_y:
            continue

        current_sequence = [statues_at_y[0]]
        longest_sequence = current_sequence[:]

        for i in range(1, len(statues_at_y)):
            gap = statues_at_y[i].x - statues_at_y[i-1].x
            if gap == 1:
                # Directly adjacent - continue sequence
                current_sequence.append(statues_at_y[i])
                if len(current_sequence) > len(longest_sequence):
                    longest_sequence = current_sequence[:]
            else:
                # Not adjacent - start new sequence
                current_sequence = [statues_at_y[i]]

        # Update best if this is better
        if len(longest_sequence) > len(best_sequence):
            best_y = y
            best_sequence = longest_sequence[:]

    return best_y, best_sequence


def generate_header(output_path: str, statues_file: str):
    """Generate yumStatues.h with statue data for GPS scanning"""

    print(f"Loading statues from {statues_file}...")
    statues = load_statues(statues_file)
    print(f"Loaded {len(statues)} statues.")

    # Find the y-coordinate with the longest adjacent sequence
    print("Finding optimal y-coordinate with longest adjacent statue sequence...")
    target_y, target_sequence = find_best_y_coordinate(statues)

    if target_y is None or not target_sequence:
        print("ERROR: No adjacent statue sequences found")
        sys.exit(1)

    print(f"Found optimal sequence at Y={target_y}: {len(target_sequence)} adjacent statues")
    print(f"  X range: {target_sequence[0].x} to {target_sequence[-1].x}")

    # Generate the header file
    print(f"Generating {output_path}...")

    with open(output_path, 'w') as f:
        f.write("// Auto-generated by yum/generate-statues.py\n")
        f.write("// DO NOT EDIT - regenerate using: python3 yum/generate-statues.py\n\n")
        f.write("#ifndef YUMSTATUES_H\n")
        f.write("#define YUMSTATUES_H\n\n")

        # Constants
        f.write(f"// Target Y coordinate for statue scanning\n")
        f.write(f"#define STATUE_TARGET_Y {target_y}\n\n")

        f.write(f"// Number of statues in the target sequence\n")
        f.write(f"#define STATUE_COUNT {len(target_sequence)}\n\n")

        # Struct definition
        f.write("// Statue target for GPS coordinate resolution\n")
        f.write("struct StatueTarget {\n")
        f.write("    int x;                    // Global X coordinate\n")
        f.write("    int displayID;            // Display ID for matching\n")
        f.write("    const char *name;         // Name (with underscores)\n")
        f.write("    const char *clothingSet;  // Semicolon-separated clothing IDs\n")
        f.write("    const char *finalWords;   // Final words (with underscores)\n")
        f.write("};\n\n")

        # Statue array
        f.write(f"// Array of statue targets at Y={target_y} for GPS scanning\n")
        f.write("static const StatueTarget STATUE_TARGETS[STATUE_COUNT] = {\n")

        for i, statue in enumerate(target_sequence):
            # Escape strings for C++
            name = escape_c_string(statue.name)
            clothing = escape_c_string(statue.clothing_set)
            final_words = escape_c_string(statue.final_words)

            f.write(f"    {{ {statue.x:6d}, {statue.display_id:3d}, ")
            f.write(f'"{name}", "{clothing}", "{final_words}" }}')

            if i < len(target_sequence) - 1:
                f.write(",")
            f.write("\n")

        f.write("};\n\n")

        f.write("#endif\n")

    print(f"Successfully generated {output_path}")


def main():
    # Determine paths relative to script location
    script_dir = os.path.dirname(os.path.abspath(__file__))
    repo_root = os.path.dirname(script_dir)  # Parent of yum/

    # Fixed paths
    statues_file = os.path.join(repo_root, "scripts", "rocketRideHistory", "statueForceLoad.txt")
    output_file = os.path.join(repo_root, "gameSource", "yumStatues.h")

    if not os.path.exists(statues_file):
        print(f"ERROR: Statue file not found: {statues_file}")
        sys.exit(1)

    generate_header(output_file, statues_file)


if __name__ == '__main__':
    main()
