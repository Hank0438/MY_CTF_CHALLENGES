def lz77_compress(data:bytes) -> list:
    """
    Compresses data using the LZ77 algorithm.
    """
    compressed_data = []
    i = 0
    data_length = len(data)

    while i < data_length:
        match_length = 0
        match_distance = 0

        # Search for the longest match in the buffer
        for j in range(max(0, i - 255), i):
            length = 0
            while (i + length < data_length) and (data[j + length] == data[i + length]) and (length < 255):
                length += 1
            
            if length > match_length:
                match_length = length
                match_distance = i - j

        # If a match was found, store it
        if match_length > 0:
            tmp = (match_distance, match_length, data[i + match_length])
            i += match_length + 1
        else:
            tmp = (0, 0, data[i])
            i += 1
        print(tmp)
        compressed_data.append(tmp)

    return compressed_data

def lz77_decompress(compressed_data:list) -> bytes:
    """
    Decompresses data using the LZ77 algorithm.
    """
    decompressed_data = []
    
    for (distance, length, char) in compressed_data:
        # Retrieve the substring from decompressed data
        start = len(decompressed_data) - distance
        for i in range(length):
            if start + i >= 0:
                decompressed_data.append(decompressed_data[start + i])
        
        # Append the next character
        if char:  # Ensure char is not empty (for the last character in the input)
            decompressed_data.append(bytes([char]))

    return b''.join(decompressed_data)

# Example usage
if __name__ == "__main__":
    original_data = "abracadabra"
    # original_data = "abcbbcbaaaaaa"
    
    original_data = original_data.encode() + b"\x00"

    # Compress the data
    compressed = lz77_compress(original_data)
    print("Compressed Data:", compressed)
    
    # Decompress the data
    decompressed = lz77_decompress(compressed)
    print("Decompressed Data:", decompressed)
