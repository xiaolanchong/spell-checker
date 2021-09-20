
# Spellchecker

## Challenge

Your goal is to implement a spelling checker. You should correct words
by finding words in the dictionary that are no more than two edits away from the input.
Here, an edit is either:
* Inserting a single letter, or
* Deleting a single letter

with the restriction that
> If the edits are both insertions or both deletions, they may not be of adjacent characters.

The input will consist of a dictionary followed by a sequence of possibly misspelt words.
Both contain words (strings of letters) of up to 50 characters. The dictionary, in free format,
is followed by a line containing just the string '\=\=='. After this, there will be zero or more
text lines containing words followed again by a line containing the string '==='. The input
is case-insensitive; print corrections from the dictionary in the case they appear in the
dictionary and unchanged words from the text lines in their original case as well.
As output, you should print the text lines with whitespace intact, with the following
changes on each word, W:
* If W is in the dictionary, print it as is.
* Otherwise, if W is not in the dictionary,
   – If no corrections can be found, print {W?}.
   – Ignore any corrections that require two edits if there is at least one that requires
     only one edit; then
   – If exactly one correction is left, print that word.
   – If more than one possible correction is left, print the set of corrections as
     {W1 W2 ...}, in the order they appear in the dictionary.

## Example

**Input**

rain spain plain plaint pain main mainly
the in on fall falls his was

\===

hte rame in pain fells

mainy oon teh lain

was hints pliant

\===

**Output**

the {rame?} in pain falls

{main mainly} on the plain

was {hints?} plaint

## Implementation

The dictionary is placed into Trie structure: https://en.wikipedia.org/wiki/Trie.

Step 1: create a collection of masks to represent possible edits. An unknown symbol is marked by ?.
E.g. 'word' after 1 edit: '?word', 'w?ord', 'ord', 'wrd', 'wor'; after 2 edits: '?od', 'or', '?or?'.
Let's the average word length is N. The number of the produced masks:

- 2 insertions: N^2/2 (more precisely is (N+1)\*N/2 taking into account prohibition of consecutive insertions)
- 2 deletions: N^2/2
- 1 deletion and 1 insertion: N^2/2
- 1 insertion: N
- 1 deletion: N

total is 3/2\*N/2 + 2\*N

The average English word length is 5 symbols, resulting in 57 masks. Even less with double letters (e.g. "letter").

Step 2: match every mask against the dictionary. An estimated number of operations

- exact match: N\*log2(26)=5N, binary search in 26 letters at each tree level (worst case when the children collection in a node is full.)
- with one arbitrary symbol: 26 + (N-1))\*log2(26)=5N+21
- with two arbitrary symbols: 26^2 + (N-2))\*log2(26)=5N+666

In reality only the 2 top tree levels are packed, and lower level child numbers quickly drop (almost halves going one level down). '??rd' is ~700 search operations, but 'wo??' is 2\*log2(26)+13\*6=88.
