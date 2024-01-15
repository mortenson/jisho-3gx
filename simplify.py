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

with open('JMdict_smol.txt', 'w') as filetowrite:
    filetowrite.write(smol)
