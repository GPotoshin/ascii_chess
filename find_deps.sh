echo "Searching for dependencies on your machine ..."
touch search.txt

echo "Searching in /usr ..."
find /usr -type d -name slog >> search.txt

find . -empty -name search.txt

echo "Search does not work for instance \n"

rm search.txt
