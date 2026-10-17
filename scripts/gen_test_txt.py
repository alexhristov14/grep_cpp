#!/usr/bin/env python3
"""Regenerates test.txt: hand-written edge cases plus ~10 MB of filler prose.

The output is deterministic (fixed seed), so rerunning gives the same file.
The filler never contains "hello", so `grep_cpp hello test.txt` still prints
only the edge-case lines shown in the README. About one line in a thousand
contains "needle", for timing a search with few matches over a big file.

Usage: python3 scripts/gen_test_txt.py [OUTPUT] [SIZE_MB]
"""

import random
import sys
from pathlib import Path

EDGE_CASES = """\
hello world
Hello World
HELLO WORLD
hello
say hello to everyone
othello is a play
helloworld
the word hello appears twice: hello

    indented hello with spaces
nothing to see here
grep is a command line utility
GREP is a command line utility
egrep and fgrep are variants

The quick brown fox jumps over the lazy dog
the quick brown fox
fox
foxes and foxglove
a.b
axb
a*b
aab
[brackets]
(parentheses)
{braces}
price is $5.00
caret ^ symbol
back\\slash
pipe | symbol
plus+sign
question?mark
line ending with period.
email: john.doe@example.com
email: jane_smith99@test.org
phone: 514-555-0123
phone: (514) 555-0199
date: 2026-10-16
version 1.2.3
ip 192.168.1.1
12345
abc123def
café
naïve résumé
日本語のテキスト
emoji 🙂 line
"""
EDGE_CASES += "    \n"  # whitespace-only line, spelled out so editors keep it

# kept last, with no trailing newline
LAST_LINE = "last line without trailing newline"

WORDS = """
the of and to in is it that was for on are as with his they at be this from
have or by one had not but what all were when we there can an your which their
said if do will each about how up out them then she many some so these would
other into has more her two like him see time could no make than first been its
who now people my made over did down only way find use may water long little
very after words called just where most know get through back much go good new
write our me man too any day same right look think also around another came come
work three word must because does part even place well such here take why things
help put years different away again off went old number great tell men say small
every found still between name should home big give air line set own under read
last never us left end along while might next sound below saw something thought
both few those always looked show large often together asked house world going
want school important until form food keep children feet land side without boy
once animals life enough took sometimes four head above kind began almost live
page got earth need far hand high year mother light parts country father let
night following picture being study second eyes soon times story boys since
white days ever paper hard near sentence better best across during today others
however sure means knew try told young miles sun ways thing whole hear example
heard several change answer room sea against top turned learn point city play
toward five using himself usually money seen car morning river garden window
mountain market letter quiet stone bridge forest kitchen silver winter summer
autumn spring harbor engine signal record buffer memory pointer kernel thread
process signal packet stream socket channel vector matrix string integer search
pattern compiler library function variable compile binary branch commit merge
""".split()

PUNCTUATION = [".", ".", ".", ".", "?", "!", ";", ","]


def sentence(rng: random.Random) -> str:
    words = rng.choices(WORDS, k=rng.randint(5, 18))
    if rng.random() < 0.05:
        words.insert(rng.randrange(len(words)), str(rng.randint(0, 99999)))
    words[0] = words[0].capitalize()
    return " ".join(words) + rng.choice(PUNCTUATION)


def line(rng: random.Random) -> str:
    text = " ".join(sentence(rng) for _ in range(rng.randint(1, 4)))
    if rng.random() < 0.001:
        text += " Somewhere in this line there is a needle."
    if rng.random() < 0.03:
        text = "    " + text
    return text


def main() -> None:
    out = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("test.txt")
    target = int(float(sys.argv[2]) * 1024 * 1024) if len(sys.argv) > 2 else 10 * 1024 * 1024

    rng = random.Random(42)
    parts = [EDGE_CASES]
    size = len(EDGE_CASES.encode())
    while size < target:
        text = "" if rng.random() < 0.08 else line(rng)  # paragraph breaks
        assert "hello" not in text.lower()
        parts.append(text + "\n")
        size += len(text) + 1
    parts.append(LAST_LINE)

    out.write_text("".join(parts), encoding="utf-8")


if __name__ == "__main__":
    main()
