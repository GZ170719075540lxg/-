import glob
import re

files = glob.glob('d:/project_ue/project_c++/Special_effects/Source/**/*.h', recursive=True) + \
    glob.glob(
        'd:/project_ue/project_c++/Special_effects/Source/**/*.cpp', recursive=True)

fixed_count = 0
for filepath in files:
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            lines = f.readlines()

        new_lines = []
        changed = False
        for line in lines:
            new_line = line
            # Fix Category = "..." missing closing quote
            if re.search(r'Category\s*=\s*"[^"]*,\s*$', new_line):
                new_line = re.sub(
                    r'(Category\s*=\s*"[^"]+),(\s*)$', r'\1",\2', new_line)
                changed = True

            # Fix DisplayName = "...))" missing closing quote
            if re.search(r'DisplayName\s*=\s*"[^"]+\)\)', new_line):
                new_line = re.sub(
                    r'(DisplayName\s*=\s*"[^"]+)\)\)', r'\1"))', new_line)
                changed = True

            new_lines.append(new_line)

        if changed:
            with open(filepath, 'w', encoding='utf-8', newline='') as f:
                f.writelines(new_lines)
            fixed_count += 1
            print(f'Fixed: {filepath}')
    except Exception as e:
        print(f'Error processing {filepath}: {e}')

print(f'Total fixed: {fixed_count} files')
