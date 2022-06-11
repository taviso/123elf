#!/bin/bash
#
# This script will run through all the smpfiles and verify the output
# matches the expected or "gold" output.
#

declare smpfiles="${ROOT:-..}/share/lotus/123.v10/smpfiles/"
declare -A blacklist=(
    [example.wk3]=1     # Example blacklist entry
    [example2.wk3]=1    # Example blacklist entry
)

function lotus_print_file()
{
    local output=$(mktemp -u)

    # Generate the output.
    if ! ../123 -e "/pf{CE}${output}~r{CE}{HOME}.{END}{HOME}~gq/qyy" "${1}"; then
        printf "error: 123 failed to generate output for %s\n" "${1}" 1>&2
        exit 1
    fi

    # Looks good, remove whitespace
    grep -v '^\s*$' "${output}" > "${2}"

    # Cleanup
    rm -f "${output}"

    return 0
}

function generate_gold_output()
{
    local base
    local real
    local i

    for i in ${smpfiles}/*.[wW][kK][13]; do
        base=$(basename "${i}")
        real=$(realpath "${base}")

        if test "${blacklist[${base}]}"; then
            continue
        fi

        if ! lotus_print_file "${i}" "${real}.gold"; then
            printf "error: 123 failed to generate output for %s\n" "${i}" 1>&2
            exit 1
        fi
    done
}


function verify_output_matches()
{
    local base
    local real
    local i

    for i in ${smpfiles}/*.[wW][kK][13]; do
        base=$(basename "${i}")
        real=$(realpath "${base}")

        if test "${blacklist[${base}]}"; then
            continue
        fi

        if ! lotus_print_file "${i}" "${real}.out"; then
            printf "error: 123 failed to generate output for %s\n" "${i}" 1>&2
            exit 1
        fi

        if ! cmp "${real}.out" "${real}.gold"; then
            printf "error: the output for %s does not match gold output\n" "${i}" 1>&2
            exit 1
        fi

        # Looks good, no need to keep it.
        rm -f "${real}.out"
    done
}

function show_help()
{
    printf "usage: %s [gold|test|help]\n" "${1}"
    printf "    gold    - generate golden outputs\n"
    printf "    test    - verify that outputs still match golden outputs\n"
    printf "    help    - print this message\n"
    exit 1
}

case "${1}" in
    gold) generate_gold_output;
          exit 0;
          ;;
    test) verify_output_matches;
          exit 0;
          ;;
    *) show_help "${0##*/}" >&2;
       ;;
esac

exit 1
