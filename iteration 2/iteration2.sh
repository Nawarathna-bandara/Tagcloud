#===============================================================================================================================================
#  CO314 : Systems and Network Programming Lab - Project I - Iteration 2
#  Group 16 : E10/049 - E10/170 - E/08/406
#  Iteration 2 leader - E/10/170

#================================================================================================================================================
# filter the unnecessary parts from the html file
  
sed -n '/<body/,/<\/body>/p' |                              # select only the text inside the body tag
sed 's/<[^>]*>//g' |                                        # remove the html tags
sed 's/&[a-zA-Z0-9]*//g' > wordlist                         # remove the escapes


#================================================================================================================================================
# count the frequencies of words and sort them to get the top 10 words

./wordfrequency english.stop < wordlist |                   # give the input to the wordfrequency executable.it return the frequency values
awk '{print $2,$1 }' |                                      # print them in stdout interchanging the columns (frequencies first and words second) 
sort -r -n |                                                # sort them according to the string numerical valuefrequencies in reverse order 
awk 'NR==1,NR==10'> top10                                   # sort the frequencies and get the words with 10 most frequent words


#===============================================================================================================================================
# store the words and frequencies inside two seperate arrays

words=($(awk '{print $2}' < "top10"))                       # add the words to array words
freqs=($(awk '{print $1}' < "top10"))                       # add the frequencies to another array


#===============================================================================================================================================
# print the svg file in xml format in the stdout


height=768                                                  # height of the svg file
width=1024                                                  # width of the svg file

echo "<svg height=\"$height\" width=\"$width\">"            # head of the svg file
echo "<title>TAG CLOUD</title>"                             # title of the svg file

for i in $(seq 0 9);do                                      # for loop is used to place the words inside the svg file.

# both x and y positions are random.(width-100) is to make sure words are not placed near the boundary and part of a word is not going to placed 
# outside of the svg image. 10 is a for a imaginary border around the svg image to limit the placement of words to that border.  

x=$(($RANDOM%($width-100)+10)) 
y=$(($RANDOM%($height-100)+10))

# font size is calculated to scale it according to the maximum and minimum values of frequencies. maximum size is 50 and minimum is 10.
# so highest frequent word will have font size of 50 and lowest will have 10.
f=$((40*(freqs[$i]-freqs[9])/(freqs[0]-freqs[9])+10))

w=${words[$i]}  # w is the word we are going to place inside svg image taken from the words array.

# place the word at position (x,y) with calculated fontsize.
echo "<text x=\"$x\" y=\"$y\" font-size=\"$f\">$w</text>"

done

# add a coloured border to the svg file
echo "<rect x=\"0\" y=\"0\" height=\"$height\" width=\"$width\" fill=\"none\" stroke=\"black\" stroke-width=\"5\" /></svg>" 

rm wordlist | rm top10                                   # we created two temporary files in this script. finally we delete them
#=================================================================================================================================================
