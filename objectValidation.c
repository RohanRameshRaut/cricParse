/*

   Rohan Ramesh Raut
   MCA-I 
   25112027
   Assignment no: 02
   clop jan 2026 cricket validation

   1) Sequence of bytes

   - check file open
   (will be done by kernel for disk and memory, after that we'll hadle using if else conditions)

   - sequence of bytes, define min(27021) and max(485588) or we'll keep max 1-2 MB
   ref min = wc -c *.yaml | grep -o '^ *[0-9]*' | sort -n | head -1
   ref max =  wc -c *.yaml | grep -o '^ *[0-9]*' | sort -n | tail -2

   - bytes checking the individual bytes for special characters allowed only
   (''', '-', ' ', '.', ':', '_') the file should not contain any other special characters

   - number of lines min(160) we'll keep it 50 ref wc -l *.yaml | sort -n | head -1 (166 352662.yaml, test match result = draw)
   max = 22253 lines, but we'll keep it 25000


   2) Sequence of lines

   - line indentation min(2 spaces) and max(11 spaces and one '-')
   - size of the line min(3 characters) and max(75) ref(wc -L *.yaml | grep -o '^ *[0-9][0-9]' | sort | uniq)
   - the line should not contain the same character throughout the line except '-'
   - the line should not end with any special character excep ('-', ':')
     ref grep -e '@$' -e '!$' -e '%$' *.yaml | wc -l
   - if line ends with character ':' then next appering line should follow the indentation rule (defined number of spaces and '-' character before the line)
   - every line should have the '\n' character except the last line

   3) Individual lines
   - the line should not start with number
   - every line should have starting index
   - next line's starting index should be strictly greater than last line's length
     (exact number will depend on whether we want to include the '\n' character or not)
   - the line should not contain the numbers only
   - line should not start and end with UPPERCASE letter
   - line should not start with ':' or any other special character except ' ' and '-'
----------------------------------------------------------------------------------------------------------------------------------------------------
Note: Mentioned validation rules/properties are stated using reference of women's t20 and test match data for min and max respectively.
Tools used: grep, sed, wc etc

 */
