import xmltodict

with open('JMdict_e.xml', 'r') as f:
	data = f.read()

# JMdict defines its tags (&n;, &v5u; and so on) as entities in its DTD.
# Recent xmltodict versions block entities by default. Only use this on the official JMdict file.
parsed = xmltodict.parse(data, disable_entities=False)

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

def get_first_gloss(senses):
  # A sense has one gloss (a string or dict) or many (a list). Use the first gloss of the first sense that has one.
  if isinstance(senses, dict):
    senses = [senses]
  if not isinstance(senses, list):
    return ''
  for sense in senses:
    glosses = sense.get('gloss', [])
    if not isinstance(glosses, list):
      glosses = [glosses]
    for gloss in glosses:
      if isinstance(gloss, dict):
        gloss = gloss.get('#text', '')
      if isinstance(gloss, str) and gloss != '':
        return gloss
  return ''

def get_reading_priorities(elem):
  # re_pri lives inside r_ele, not on the entry. It can be a string or a list.
  if isinstance(elem, dict):
    elem = [elem]
  if not isinstance(elem, list):
    return []
  pri = elem[0].get('re_pri', [])
  if isinstance(pri, str):
    return [pri]
  return pri

for entry in parsed['JMdict']['entry']:
  # loan word
  if any(p in ('gai1', 'gai2') for p in get_reading_priorities(entry.get('r_ele', {}))):
    continue

  kanji = get_singleton(entry.get('k_ele', {}), 'keb')
  furigana = get_singleton(entry.get('r_ele', {}), 'reb')
  definition = get_first_gloss(entry.get('sense', []))

  if definition == '' or furigana == '':
    continue

  # katakana
  if ord(u"\u30a0") <= ord(furigana[0]) <= ord(u"\u30ff"):
    continue

  lines.append('|'.join([furigana, kanji, definition]))

lines.sort()

smol = '\n'.join(lines)

# The first line is a header. The plugin skips it and binary searches the sorted lines after it.
# Lines are sorted by code point, which matches the byte order of UTF-8.
smol = 'jisho1\n' + smol

with open('JMdict_smol.txt', 'w') as filetowrite:
    filetowrite.write(smol)
