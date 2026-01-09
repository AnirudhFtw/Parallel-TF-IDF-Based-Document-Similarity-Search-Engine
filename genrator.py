import os
import random
import string
from typing import List

# --- Configuration ---
NUM_FILES = 20
FOLDERS = {
    "small_files": (1, 5),      # File size in KB (Kilobytes)
    "medium_files": (50, 100),  # File size in KB
    "large_files": (500, 1000)  # File size in KB (up to 1MB)
}
# A simple "meaningful" corpus to generate text
CORPUS = [
    "The quick brown fox jumps over the lazy dog.",
    "A journey of a thousand miles begins with a single step.",
    "To be or not to be, that is the question.",
    "The early bird catches the worm.",
    "All that glitters is not gold.",
    "Success is not final, failure is not fatal: it is the courage to continue that counts.",
    "Python is a versatile and powerful programming language.",
    "Data generation scripts are essential for testing applications and file systems.",
    "Remember to always back up your important data regularly.",
    "A well-organized file system improves efficiency and reduces clutter."
]

def generate_random_text(min_size_kb: int, max_size_kb: int) -> str:
    """Generates a string of text with a size within the specified KB range."""
    # Target size is chosen randomly between min_size_kb and max_size_kb
    target_size_bytes = random.randint(min_size_kb * 1024, max_size_kb * 1024)
    text = []
    current_size = 0
    
    while current_size < target_size_bytes:
        # Pick a random sentence from the corpus
        sentence = random.choice(CORPUS)
        # Randomly choose whether to repeat the sentence for more 'meaningful' data
        if random.random() < 0.2:
            sentence = " ".join([sentence] * random.randint(2, 5))
        
        # Add a file separator (newline and space)
        segment = sentence + "\n "
        text.append(segment)
        current_size += len(segment.encode('utf-8'))
        
        # Add some random words sometimes to vary the content
        if random.random() < 0.5:
             random_word = ''.join(random.choices(string.ascii_letters + string.digits, k=random.randint(5, 15)))
             text.append(random_word + " ")
             current_size += len(random_word.encode('utf-8'))
        
    # Trim the text to ensure it's not *too* much over the max size
    final_text = "".join(text)
    # This slicing is an approximation since we're working with multi-byte UTF-8
    # but it helps keep the file close to the target size.
    final_text_bytes = final_text.encode('utf-8')[:target_size_bytes] 
    return final_text_bytes.decode('utf-8', errors='ignore')


def create_files_in_folder(folder_name: str, num_files: int, min_kb: int, max_kb: int):
    """Creates a specified number of files in a folder with random content."""
    
    # 1. Create the folder if it doesn't exist
    os.makedirs(folder_name, exist_ok=True)
    print(f"📁 Created/Ensured directory: '{folder_name}'")
    
    # 2. Generate the files
    for i in range(1, num_files + 1):
        filename = f"document_{i:02d}_{min_kb}to{max_kb}KB.txt"
        filepath = os.path.join(folder_name, filename)
        
        # Generate the random text
        content = generate_random_text(min_kb, max_kb)
        
        # Write the content to the file
        try:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(content)
            
            # Optional: Print file info
            file_size_bytes = os.path.getsize(filepath)
            print(f"  📝 Created file: '{filename}' ({file_size_bytes / 1024:.2f} KB)")
        except IOError as e:
            print(f"  ❌ Error writing file {filepath}: {e}")

def main():
    """Main function to orchestrate folder and file creation."""
    print("--- Starting File Generation Script ---")
    
    # Iterate over the defined folders and their size ranges
    for folder, (min_kb, max_kb) in FOLDERS.items():
        print("-" * 30)
        create_files_in_folder(folder, NUM_FILES, min_kb, max_kb)
    
    print("-" * 30)
    print("✅ All files and folders have been created successfully!")
    print("The files are now available in the current directory.")

if __name__ == "__main__":
    main()