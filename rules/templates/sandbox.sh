#!/bin/bash

################################################################################
# Usage:
#   <filename> [-h|-?|--help] [-i|--install] [-v|--verbose] 
#     [--name=<project_name>] [--make-build-script]
################################################################################

usage() {
    echo "Usage:"
    printf ""
}

CONST_INSTALL_PATH="/usr/local/sbin"
CONST_PROJECT_NAME=
CONST_PASSWORD=
ANSWER="n"

FLAG_VERBOSE=0
FLAG_NEWSANDBOX=0
FLAG_MAKE_BUILD_SCRIPT=0

_say() {
    if [ $FLAG_VERBOSE -ne 0 ]; then
        case $2 in
            -E|--error)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[31;1m%s\e[0m\n' "$1"
            ;;
            -W|--warning)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[33;1m%s\e[0m\n' "$1"
            ;;
            -h|--head-line)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[32;3;1m%s\e[0m\n' "$1"
            ;;
            -i|--info)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[35m%s\e[0m\n' "$1"
            ;;
            *)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[0;36m%s \e[0;32m%s \e[0m\n' "$1"
            ;;
        esac
    fi
}

_say1() {
    case $2 in
            -E|--error)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[31;1m%s\e[0m\n' "$1"
            ;;
            -W|--warning)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[33;1m%s\e[0m\n' "$1"
            ;;
            -h|--head-line)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[32;3;1m%s\e[0m\n' "$1"
            ;;
            -i|--info)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[35m%s\e[0m\n' "$1"
            ;;
            *)
                printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
                printf '\e[0;36m%s \e[0;32m%s \e[0m\n' "$1"
            ;;
        esac
}

die() {
    _say1 "$1" --error
    exit 1
}

_ask() {
    printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
    printf '\e[0;36m%s \e[0;32m%s \e[0m' "Do you want $1? ([y|]n):"
    read ANSWER
    if [ $ANSWER != "y" ]; then
        ANSWER="n"
    else
        ANSWER="y"
    fi
}

create_sandbox() {
    _say 'Start creating a sandbox' --head-line
    if [ -z $CONST_PROJECT_NAME ]; then
        printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
        printf '\e[0;36m%s \e[0;32m%s \e[0m' "Enter the project name:"
        read CONST_PROJECT_NAME
    fi
    _say1 "[Info]: Project name is \"$CONST_PROJECT_NAME\"" --info
    if [ ! -d $PWD/$CONST_PROJECT_NAME ]; then
        _say "Create directory: $PWD/$CONST_PROJECT_NAME/src"
        mkdir -p $PWD/$CONST_PROJECT_NAME/src
        _say "Create directory: $PWD/$CONST_PROJECT_NAME/include"
        mkdir -p $PWD/$CONST_PROJECT_NAME/include
        _say "Create directory: $PWD/$CONST_PROJECT_NAME/rsync"
        mkdir -p $PWD/$CONST_PROJECT_NAME/rsync
        # README.md ------------------------------------------------------------
        _say "Create file: $PWD/$CONST_PROJECT_NAME/README.md"
        touch $PWD/$CONST_PROJECT_NAME/README.md
        echo "# $CONST_PROJECT_NAME" >> "$PWD/$CONST_PROJECT_NAME/README.md"
        echo "## Description" >> "$PWD/$CONST_PROJECT_NAME/README.md"
        echo "## Authors" >> "$PWD/$CONST_PROJECT_NAME/README.md"
        # Makefile -------------------------------------------------------------
        _say "Create file: $PWD/$CONST_PROJECT_NAME/Makefile"
        touch $PWD/$CONST_PROJECT_NAME/Makefile
        printf "SUBDIRS = util\nSUBDIRS += src\nTESTDIRS = tests\n\n" >> $PWD/$CONST_PROJECT_NAME/Makefile
        printf ".PHONY : all test check \$(SUBDIRS) \$(TESTDIRS)\nall : \$(SUBDIRS)\n\n" >> $PWD/$CONST_PROJECT_NAME/Makefile
        echo 'test : all $(TESTDIRS)' >> $PWD/$CONST_PROJECT_NAME/Makefile
        echo >> $PWD/$CONST_PROJECT_NAME/Makefile
        echo 'check : test' >> $PWD/$CONST_PROJECT_NAME/Makefile
        echo >> $PWD/$CONST_PROJECT_NAME/Makefile
        printf "\$(SUBDIRS):\n\t\$(MAKE) -C \$@ \$(MAKECMDGOALS)\n\n" >> $PWD/$CONST_PROJECT_NAME/Makefile
        printf "\$(TESTDIRS):\n\t\$(MAKE) -C \$@ \$(MAKECMDGOALS)\n\n" >> $PWD/$CONST_PROJECT_NAME/Makefile
        printf ".PHONY : clean\nclean : \$(SUBDIRS) \$(TESTDIRS)" >> $PWD/$CONST_PROJECT_NAME/Makefile
        # src/Makefile ---------------------------------------------------------
        _say "Create file: $PWD/$CONST_PROJECT_NAME/src/Makefile"
        touch $PWD/$CONST_PROJECT_NAME/src/Makefile
        echo 'SUBDIRS = $(shell ls -D)' >> $PWD/$CONST_PROJECT_NAME/src/Makefile
        echo '.PHONY : all test check $(SUBDIRS)' >> $PWD/$CONST_PROJECT_NAME/src/Makefile
        echo 'all : $(SUBDIRS)' >> $PWD/$CONST_PROJECT_NAME/src/Makefile
        echo 'test : all' >> $PWD/$CONST_PROJECT_NAME/src/Makefile
        echo 'check : test' >> $PWD/$CONST_PROJECT_NAME/src/Makefile
        printf "\n\$(SUBDIRS):\n\t\$(MAKE) -C \$@ \$(MAKECMDGOALS)\n\n" >> $PWD/$CONST_PROJECT_NAME/src/Makefile
        echo '.PHONY : clean' >> $PWD/$CONST_PROJECT_NAME/src/Makefile
        echo 'clean : $(SUBDIRS)' >> $PWD/$CONST_PROJECT_NAME/src/Makefile
        # make.sh --------------------------------------------------------------
        _say "Create file: $PWD/$CONST_PROJECT_NAME/make.sh"
        touch $PWD/$CONST_PROJECT_NAME/make.sh
        echo '#!/usr/bin/env bash' >> $PWD/$CONST_PROJECT_NAME/make.sh
        echo 'export PRODUCT=' >> $PWD/$CONST_PROJECT_NAME/make.sh
        echo 'export HLQ=' >> $PWD/$CONST_PROJECT_NAME/make.sh
        echo 'make $1 $2 $3 $4 $5' >> $PWD/$CONST_PROJECT_NAME/make.sh
        _say1 "[Info]: Do not forget specify PRODUCT and HLQ in \"$PWD/$CONST_PROJECT_NAME/make.sh\""
        # .gitignore -----------------------------------------------------------
        _say "Create file: $PWD/$CONST_PROJECT_NAME/.gitignore"
        # rsync/exclude.txt ----------------------------------------------------
        _say "Create file: $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt"
        touch $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt
        echo '.*' >> $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt
        echo '.vscode/*' >> $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt
        echo '*.o' >> $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt
        echo '*.d' >> $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt
        echo '*.txt' >> $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt
        echo 'rsync/*' >> $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt
        echo '*.test' >> $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt
        echo 'rcc.util/*' >> $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt
        echo 'util' >> $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt
        echo 'makeinc' >> $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt
        echo '*.s' >> $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt
        echo 'build.sh' >> $PWD/$CONST_PROJECT_NAME/rsync/exclude.txt
        # rsync/sync.sh --------------------------------------------------------
        _say "Create file: $PWD/$CONST_PROJECT_NAME/rsync/sync.sh"
        touch $PWD/$CONST_PROJECT_NAME/rsync/sync.sh
        echo '#!/bin/sh' >> $PWD/$CONST_PROJECT_NAME/rsync/sync.sh
        echo 'cd ..' >> $PWD/$CONST_PROJECT_NAME/rsync/sync.sh
        printf 'rsync --protocol=26 --recursive --verbose --delete --copy-links --size-only --checksum --compress --exclude-from=./rsync/exclude.txt . rsync://' >> $PWD/$CONST_PROJECT_NAME/rsync/sync.sh
        _ask "specify sandbox on z/OS USS"
        if [ $ANSWER = "y" ];then
            read -p "Enter \"rsync\" link: " TEMP_ANSWER
            printf "$TEMP_ANSWER" >> $PWD/$CONST_PROJECT_NAME/rsync/sync.sh
            _say1 "[Info]: rsync link is \"rsync://$TEMP_ANSWER\""
        fi
        chmod +x "$PWD/$CONST_PROJECT_NAME/rsync/sync.sh"
    else 
        _say1 "[Info]: $CONST_PROJECT_NAME already exist" --info
    fi
}

create_build_script() {
    _say 'Start creating "build.sh" script' --head-line
    if [ -z $CONST_PROJECT_NAME ]; then
        printf '\e[0;36m%s \e[0;32m%s \e[0m' $(date '+%T')
        printf '\e[0;36m%s \e[0;32m%s \e[0m' "Enter the project name:"
        read CONST_PROJECT_NAME
    fi
    touch "$PWD/$CONST_PROJECT_NAME/build.sh"
    read -p "Enter login for SSH: " TEMP_ANSWER
    CONNECTION=$TEMP_ANSWER
    read -p "Enter server for SSH: " TEMP_ANSWER
    CONNECTION="$CONNECTION@$TEMP_ANSWER"
    _say1 "[Info]: Project name is $CONNECTION" --info
    read -p "Enter absolute path to sandbox on USS: " TEMP_ANSWER
    PATH=$TEMP_ANSWER
    _say1 "[Info]: Path to USS $PATH" --info
    COMMAND_DEFAULT="ssh $CONNECTION 'cd $PATH && ./make.sh'"
    COMMAND_CLEAR="ssh $CONNECTION 'cd $PATH && ./make.sh clear'"
    echo '#!/bin/bash' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo 'process_flags() {' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo '    while :; do' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo '        case $1 in' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo '            -c|--clear)' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo "                $COMMAND_CLEAR" >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo "                exit" >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo '                ;;' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo '           -?*)' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo "                printf '\\e[33;1m[WARNING]: Unknown option (ignored): %s\\e[0m\\n\' "\$1" >&2" >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo '               ;;' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo '            *)' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo '                break' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo '        esac' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo '        shift' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo '    done' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo '}' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo 'process_flags $*' >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    echo "$COMMAND_DEFAULT" >> "$PWD/$CONST_PROJECT_NAME/build.sh"
    chmod +x "$PWD/$CONST_PROJECT_NAME/build.sh"
}

process_flags() {
    while :; do
        case $1 in
            -h|-\?|--help)
                usage $0
                exit
                ;;
            -i|--install)
                sudo cp -f $0 $CONST_INSTALL_PATH
                exit
                ;;
            -v|--verbose)
                FLAG_VERBOSE=1
                shift
                break
                ;;
            --name=?*)
                CONST_PROJECT_NAME=${1#*=}
                FLAG_NEWSANDBOX=1
                shift
                ;;
            --name=)
                die "[Fatal error]: \"--name\" requires a non-empty option argument."
                ;;
            --make-build-script)
                FLAG_MAKE_BUILD_SCRIPT=1
                shift
                ;;
            --)
                shift
                break
                ;;
            -?*)
                printf '\e[33;1m[WARNING]: Unknown option (ignored): %s\e[0m\n' "$1" >&2
                ;;
            *)
                break
        esac
        shift
    done
}

# MAIN PROCEDURE ###############################################################

process_flags $*
if [ $FLAG_NEWSANDBOX -ne 0 -a ! -d $PWD/$CONST_PROJECT_NAME ]; then
    create_sandbox
else
    _ask "create new sandbox"
    if [ $ANSWER = "y" ];then
        create_sandbox
    fi
fi

if [ -d $PWD/$CONST_PROJECT_NAME ]; then
    if [ ! -f "$PWD/$CONST_PROJECT_NAME/build.sh" ]; then
        if [ $FLAG_MAKE_BUILD_SCRIPT -ne 0 ]; then
            create_build_script
        else
            _ask "create \"build.sh\" script"
            if [ $ANSWER = "y" ]; then
                create_build_script
            fi
        fi
    fi
fi

exit 0
