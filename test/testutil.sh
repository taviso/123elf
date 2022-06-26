#!/bin/bash
#
# This script contains helper functions for verifying 1-2-3 functionality.
#

function runmacro()
{
    if ! eval ../123 "${@}"; then
        printf "error: 123 did not complete successfully\n" 1>&2
        exit 1
    fi
}

# verifyexist file [false]
function verifyexist()
{
    if ! test -e "${1}"; then
        if test ${2:-true} == "true"; then
            printf "error: the file %s did not exist\n" "${1}" 1>&2
            exit 1
        fi
    else
        if test ${2:-true} == "false"; then
            printf "error: the file %s did exist\n" "${1}" 1>&2
            exit 1
        fi
    fi
}
# verifycontents file contents
function verifycontents()
{
    local temp=$(mktemp -u)
    local orig="${1}"

    verifyexist "${orig}"
    shift
    printf -- "${*}" >> "${temp}"

    verifymatch "${orig}" "${temp}"

    # Clean up.
    rm -f "${temp}"
}

function verifymatch()
{
    verifyexist "${1}"
    verifyexist "${2}"

    if ! cmp "${1}" "${2}"; then
        printf "error: the contents of %s and %s did not match\n" "${1}" "${2}" 1>&2
        diff -ruN "${1}" "${2}"
        exit 1
    fi
}

# verifysum filename checksum size
function verifysum()
{
    local sum

    verifyexist "${1}"
    if ! sum=$(cksum < "${1}"); then
        printf "error: failed to get checksum of file\n" 1>&2
        exit 1
    fi
    if ! test "${sum}" == "${2} ${3}"; then
        printf "error: checksum of %s didn't match\n" "${1}" 1>&2
        exit 1
    fi
}

function starttest()
{
    printf "Testing %s..." "${1}"

    if test ${#} -gt 1; then
        if ! type "${2}" &> /dev/null; then
            printf "skipped (no %s)\n" "${2}"
            return 1
        fi
    fi

    return 0
}

function endtest()
{
    printf "ok\n"
    # Cleanup any temporary files specified
    rm -f -- "${@}"
}
