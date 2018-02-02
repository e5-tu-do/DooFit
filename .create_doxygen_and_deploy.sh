#!/bin/sh

set -e # exit with nonzero exit code if anything fails

echo 'Generating Doxygen code documentation...'
echo 'Using the following version of doxygen:'
doxygen --version
doxygen doxygen.conf

if [ -d "html" ] && [ -f "html/index.html" ];
then
	# add remote ssh-key to key-storage
	# first add remote host to known hosts
	ssh-keyscan -t rsa $DEPLOY_HOST 2> /dev/null | sort -u - ~/.ssh/known_hosts -o ~/.ssh/known_hosts

	# decrypt private shh key
	openssl aes-256-cbc -K $encrypted_2adf16dc08ac_key -iv $encrypted_2adf16dc08ac_iv -in id_rsa_doofit.enc -out id_rsa_doofit -d

	# start ssh-agent and add the key
	eval "$(ssh-agent -s)"
	chmod 600 id_rsa_doofit
	ssh-add id_rsa_doofit

	# upload site
	rsync -rq --delete --exclude=".*" ~/out/ $DEPLOY_USER@$DEPLOY_HOST:$DEPLOY_PATH
else
    echo '' >&2
    echo 'Warning: No documentation (html) files have been found!' >&2
    echo 'Warning: Not going to push the documentation to docs.e5.physik.tu-dortmund.de' >&2
    exit 1
fi
