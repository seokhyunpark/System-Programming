# Show file similarity
import difflib
import sys

main_file = sys.argv[1]
sub_file = sys.argv[2]

main_contents = open(main_file, 'r').read()
sub_contents = open(sub_file, 'r').read()

# Convert file contents to bytes
main_bytes = bytes(main_contents, 'utf-8')
sub_bytes = bytes(sub_contents, 'utf-8')

# Convert file bytes to a list
main_bytes_list = list(main_bytes)
sub_bytes_list = list(sub_bytes)

# Create a SequenceMatcher object
sm = difflib.SequenceMatcher(None, main_bytes_list, sub_bytes_list)
# Calculate the similarity ratio as a percentage
similar = sm.ratio() * 100

print(similar)
