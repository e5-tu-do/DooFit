#!/bin/sh

echo 'Generating Doxygen code documentation...'
echo 'Using the following version of doxygen:'
doxygen --version
doxygen doxygen.conf

if [ -d "html" ] && [ -f "html/index.html" ];
then
	# this line can be delted when the dns entry is available
	echo "$DOCS_HOSTS" >> /etc/hosts
	cat /etc/hosts
	echo 'Uploading documentation to docs.e5.physik.tu-dortmund.de using curl...'
	#first try to upload a single file
	cd html && curl -f -u $DOCS_USER:$DOCS_PASSWORD -k -L -T index.html $DOCS_URL/index.html
	find . -type f -exec curl -f -u $DOCS_USER:$DOCS_PASSWORD -k -L -T {} $DOCS_URL{} \;
else
    echo '' >&2
    echo 'Warning: No documentation (html) files have been found!' >&2
    echo 'Warning: Not going to push the documentation to docs.e5.physik.tu-dortmund.de' >&2
    exit 1
fi
