from pypdf import PdfReader
import sys

if len(sys.argv) < 2:
    print('Usage: pdf_extract.py <pdf_path>')
    sys.exit(1)

reader = PdfReader(sys.argv[1])
text = []
for i, page in enumerate(reader.pages):
    try:
        text.append(page.extract_text() or '')
    except Exception as e:
        text.append('')

print('\n\n-----PDF TEXT START-----\n')
print('\n\n'.join(text))
print('\n\n-----PDF TEXT END-----\n')
