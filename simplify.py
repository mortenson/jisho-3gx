import xmltodict

with open('JMdict_e.xml', 'r') as f:
	data = f.read()

parsed = xmltodict.parse(data)

lines = []

def get_singleton(elem, selector):
  if isinstance(elem, dict):
    elem = [elem]
  if isinstance(elem, list):
    for elem_item in elem:
      child = elem_item.get(selector, {})
      if isinstance(child, dict):
        return child.get('#text', '')
      if isinstance(child, str):
        return child
  if isinstance(elem, str):
    return elem
  return ''

def utf8len(s):
  return len(s.encode('utf-8'))

for entry in parsed['JMdict']['entry']:
  # loan word
  if entry.get('re_pri', '') == 'gai1' or entry.get('re_pri', '') == 'gai2':
    continue

  kanji = get_singleton(entry.get('k_ele', {}), 'keb')
  furigana = get_singleton(entry.get('r_ele', {}), 'reb')
  definition = get_singleton(entry.get('sense', {}), 'gloss')

  # katakana
  if ord(u"\u30a0") <= ord(furigana[0]) <= ord(u"\u30ff"):
    continue

  if definition == '' or furigana == '':
    continue

  lines.append('|'.join([furigana, kanji, definition]))

lines.sort()

smol = '\n'.join(lines)

# Create index

dictIndex = "index"
chars = ["あ", "い", "う", "え", "お", "か", "き", "く", "け", "こ", "が", "ぎ", "ぐ", "げ", "ご", "さ", "し", "す", "せ", "そ", "ざ", "じ", "ず", "ぜ", "ぞ", "た", "ち", "つ", "て", "と", "だ", "づ", "で", "ど", "な", "に", "ぬ", "ね", "の", "は", "ひ", "ふ", "へ", "ほ", "ば", "び", "ぶ", "べ", "ぼ", "ぱ", "ぴ", "ぷ", "ぺ", "ぽ", "ま", "み", "む", "め", "も", "や", "ゆ", "よ", "ら", "り", "る", "れ", "ろ", "わ", "を", "ん"]
for char in chars:
  charIndex = smol.find("\n" + char)
  if charIndex == -1:
    continue
  dictIndex += "|" + char + ":" + str(utf8len(smol[:charIndex]))

smol = dictIndex + '\n' + smol

with open('JMdict_smol.txt', 'w') as filetowrite:
    filetowrite.write(smol)
