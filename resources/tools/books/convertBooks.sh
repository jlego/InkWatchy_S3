#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BOOK_SRC="$SCRIPT_DIR/../../personal/books"
BOOK_OUT="$SCRIPT_DIR/out"
BOOK_DEST="$SCRIPT_DIR/../fs/littlefs/book"

rm -rf "$BOOK_OUT" 2>/dev/null || true
mkdir -p "$BOOK_OUT/conf/"
touch "$BOOK_OUT/conf/.keep"

if ! python3 -c "import fitz" 2>/dev/null; then
    echo "Installing pymupdf..."
    pip3 install pymupdf --quiet
fi

if [ ! -d "$BOOK_SRC" ]; then
    echo "No books directory: $BOOK_SRC"
    exit 1
fi

count=0
for file in "$BOOK_SRC"/*; do
    [ -f "$file" ] || continue
    case "${file##*.}" in
        epub|pdf) ;;
        *) continue ;;
    esac
    count=$((count + 1))
    file_name=$(basename "$file")
    file_no_ext="${file_name%.*}"
    safe_name=$(python3 -c 'import re,sys; s=re.sub(r"[^a-zA-Z0-9_\u4e00-\u9fff]","_",sys.argv[1]); print(s if s else "book_'$count'")' "$file_no_ext")
    echo "Processing [$count]: $file_name -> $safe_name"
    python3 -c '
import fitz, sys
doc = fitz.open(sys.argv[1])
text = ""
for page in doc:
    text += page.get_text()
text = " ".join(text.split())
with open(sys.argv[2], "w", encoding="utf-8") as f:
    f.write(text)
print(f"  Extracted {len(text)} chars")
' "$file" "$BOOK_OUT/$safe_name"
done

if [ "$count" -eq 0 ]; then
    echo "No .epub or .pdf files found in $BOOK_SRC"
    exit 0
fi

echo "Processed $count book(s)"
rm -rf "$BOOK_DEST" 2>/dev/null || true
mv "$BOOK_OUT" "$BOOK_DEST"
echo "Done. Books ready in $BOOK_DEST"
echo "Next: cd resources/tools/fs && bash createFs.sh"