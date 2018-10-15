# IPS

Our projects for IPS in the winter semester 2018/2019 at VUT FIT.

Credit goes to:<br>
Sabína Gregušová<br>
Tomáš Sasák


Project 1: ?/?<br>
Project 2: ?/?

## Parallel SED
PSED behaves exactly same like original SED. But the diffrence, is that every single requested regex owns one own thread and works with the thread until input pipeline is empty.
### Execution
PSED run commands:
```bash
./psed REGEX1 REGEX_REPLACE1 [ REGEX2 REGEX_REPLACE2, ... , REGEXN REGEX_REPLACEN ] < requested_text_file
```
Output of the PSED is printed in exact order how they were requested and for each line.

Example: We have text file named example.txt.
```
Ahoj, tohle je pokus
jestli to bude fungovat 23
211 je vysledek
```
PSED example execution:
```
./psed '(.*) tohle (.*)' '$2 XX $1' 'je ' 'byl ' <example
```
The output will be:
```
je pokus XX Ahoj,
Ahoj, tohle byl pokus
jestli to bude fungovat 23
jestli to bude fungovat 23
211 je vysledek
211 byl vysledek
```

