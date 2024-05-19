def downsample_text_file(input_file_path, output_file_path):
    with open(input_file_path, 'r') as input_file:
        with open(output_file_path, 'w') as output_file:
            line_count = 0
            for line in input_file:
                if (line_count + 1) % 100000 == 0:
                    output_file.write(line)
                line_count += 1

# Example usage
input_file_path = 'dictionaries/rockyou.txt'
output_file_path = 'dictionaries/test_plains.txt'
downsample_text_file(input_file_path, output_file_path)
