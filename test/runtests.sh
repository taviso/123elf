#!/bin/bash
#
# This script is used to check 123 is functioning as expected.
#

declare smpfiles="${ROOT:-..}/share/lotus/123.v10/smpfiles/"

source macutil.sh
source testutil.sh

function lotus_print_file()
{
    local output=$(mktemp -u)

    # Generate the output.
    runmacro -e "/pf{CE}${output}~r{CE}{HOME}.{END}{HOME}~gq/qyy" "${1}"

    # Looks good, trim whitespace
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
        lotus_print_file "${i}" "${real}.txt"
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

        starttest "${base}" md5sum && {
            lotus_print_file "${i}" "${real}.txt"
            if ! md5sum --quiet --check --ignore-missing smpfiles.txt; then
                printf "error: file %s did not print correctly\n" "${i}" 1>&2
                exit 1
            fi
            endtest "${real}.txt"
        }
    done
}

# Usage: value expected format width
function verify_result_contents()
{
    local output=$(mktemp -u)
    local result
    local macro

    starttest "${1}"

    # Copy the contents specified using the format specified.
    printf -v macro -- "-e '{HOME}%s~{CONTENTS B1,A1,%u,%u}'" "${1}" "${4:-12}" "${3:-127}"

    # Add commands to save the result.
    macro+=$(writerange "${output}" "@TRIM(B1)")
    macro+=$(quit)

    runmacro ${macro}
    verifycontents "${output}" "${2}"
    endtest

    # Clean up
    rm -f "${output}"
}

function verify_file_ops()
{
    local output=$(mktemp -u --suffix=.wk3)
    local result=$(mktemp -u)
    local lotdir=$(mktemp -d)
    local scrdmp=$(mktemp -u)
    local macro

    starttest "/File Save" && {
        # Generate some test data.
        printf -v macro -- "-e '/df.{END}{D}~~~~'"
        macro+=$(saveas "${output}")
        macro+=$(quit)
        runmacro "${macro}"
        verifyexist "${output}"
        endtest
    }

    starttest "/File Retrieve" && {
        macro=$(retrieve "${output}")
        macro+=$(writerange "${result}" "@STRING(@SUM(A1..A8192), 0)")
        macro+=$(quit)
        runmacro "${macro}"
        verifycontents "${result}" 33550336
        endtest
    }

    starttest "/File Combine" && {
        printf -v macro -- "-e '/fcce{CE}%s~/fcae{CE}%s~'" "${output}" "${output}"
        macro+=$(writerange "${result}" "@STRING(@SUM(A1..A8192), 0)")
        macro+=$(quit)
        runmacro "${macro}"
        verifycontents "${result}" 67100672
        endtest
    }

    starttest "/File Xtract" && {
        printf -v macro -- "-e '/df.{END}{D}~~~~/fxv{CE}%s~{D 10}~r'" "${output}"
        macro+=$(retrieve "${output}")
        macro+=$(writerange "${result}" "@STRING(@SUM(A1..A8192), 0)")
        macro+=$(quit)
        runmacro "${macro}"
        verifycontents "${result}" 55
        endtest
    }

    starttest "/File Open" && {
        printf -v macro -- "-e '/foa{CE}%s~y~'" "${output}"
        macro+=$(quit)
        runmacro "${macro}"
        endtest
    }

    starttest "/File Admin" && {
        printf -v macro -- "-e '/fatw{CE}%s~~'" "${output}"
        macro+=$(writerange "${result}" "A1")
        macro+=$(quit)
        runmacro "${macro}"
        verifycontents "${result}" "${output##*/}"
        endtest
    }

    starttest "/File Erase" && {
        printf -v macro -- "-e '/few{CE}%s~y~'" "${output}"
        macro+=$(quit)
        runmacro "${macro}"
        verifyexist "${output}" false
        endtest
    }

    starttest "/File List" && {
        printf -v macro -- "-e '/flo{CE}%s/*'~~" "/tmp"
        macro+=$(quit)
        runmacro "${macro}"
        endtest
    }

    starttest "/File Import" && {
        seq 123 456 > ${result}
        printf -v macro -- "-e '/fin{CE}%s~'" "${result}"
        macro+=$(writerange "${result}" "@STRING(@SUM(A1..A8192), 0)")
        macro+=$(quit)
        runmacro "${macro}"
        verifycontents "${result}" 96693
        endtest
    }

    starttest "/File Dir" && {
        printf -v macro -- "-e '/fd{CE}%s~'" "/tmp"
        macro+=$(quit)
        runmacro "${macro}"
        endtest
    }

    starttest "/File New" && {
        printf -v macro -- "-e '/fna{CE}%s~'" "${output}"
        macro+=$(quit)
        runmacro "${macro}"
        verifyexist "${output}" false
        endtest
    }

    # Check that wildcards work
    starttest "wildcards" && {
        verifyexist "${lotdir}"
        for i in {a,b,c}{.txt,.wk3,.wk1}; do
            touch ${lotdir}/${i}
        done
        printf -v macro -- " -e '/fd{CE}%s~/fr'" ${lotdir}
        macro+=$(screendump)
        macro+=$(escape 4)
        macro+=$(quit)
        LOTUS_SCREEN_DUMP="${scrdmp}" COLUMNS=80 runmacro "${macro}"
        verifyexist "${scrdmp}"

        # Pull out the file list
        sed 's/\s\+/ /g;3q;d' "${scrdmp}" > "${result}"

        # Check they're all listed
        verifycontents "${result}" "a.wk1 a.wk3 b.wk1 b.wk3 \n"
        endtest "${scrdmp}"
    }

    starttest "/File Import w/Long Name" && {
        verifyexist "${lotdir}"
        seq 1 10 > ${lotdir}/this-is-a-very-long-name-longer-than-expected-it-keeps-going.csv
        printf -v macro -- " -e '/fin{CE}%s/*.csv~'" ${lotdir}
        macro+=$(screendump)
        macro+=$(enter)
        macro+=$(writerange "${result}" "@STRING(@SUM(A1..A8192), 0)")
        macro+=$(quit)
        LOTUS_SCREEN_DUMP="${scrdmp}" COLUMNS=80 runmacro "${macro}"
        verifycontents "${result}" 55
        verifyexist "${scrdmp}"

        # Pull out the file list
        sed 's/\s\+/ /g;3q;d' "${scrdmp}" > "${result}"

        # Check the long file is in there (truncated, but thats okay).
        verifycontents "${result}" "this-is-a-very-lon \n"
        endtest "${scrdmp}"
    }

    starttest "/File Admin Reservation" lsof && {
        # Generate some test data.
        macro=$(sendkeys  "/df.{END}{D}~~~~")
        macro+=$(saveas "${output}")
        # Check that lsof can see the lock
        macro+=$(system "lsof -Fl -a -f -- ${output}")
        macro+=$(writerange "${result}" '@STRING(@INFO("osreturncode") / 2^8, 0)')
        macro+=$(quit)

        runmacro "${macro}"

        # Check the return code indicates a lock was present.
        verifycontents "${result}" "0"

        macro=$(retrieve "${output}")
        macro+=$(sendkeys "/farr")
        # Check that lsof cannot see a lock
        macro+=$(system "lsof -Fl -a -f -- ${output}")
        macro+=$(writerange "${result}" '@STRING(@INFO("osreturncode") / 2^8, 0)')
        macro+=$(quit)

        runmacro "${macro}"

        # Check the return code indicates a lock was present.
        verifycontents "${result}" "1"

        macro=$(retrieve "${output}")
        macro+=$(sendkeys "/farr")
        macro+=$(screendump)
        macro+=$(sendkeys "/farg")
        # Check that lsof can see a lock
        macro+=$(system "lsof -Fl -a -f -- ${output}")
        macro+=$(writerange "${result}" '@STRING(@INFO("osreturncode") / 2^8, 0)')
        macro+=$(quit)

        LOTUS_SCREEN_DUMP="${scrdmp}" COLUMNS=80 runmacro "${macro}"

        # Check the return code indicates a lock was present.
        verifycontents "${result}" "0"

        # Check that there was a "RO" indicator when the lock was released.
        sed 's/\s\+/ /g;$!d' "${scrdmp}" > "${result}"

        verifycontents "${result}" "${output##*/} RO CMD \n"
        endtest "${result}" "${output}"
    }

    # Read a few lines of varying length
    starttest "/File Import Text" && {
        printf "%c" {A..Z} $'\n' > ${result}
        printf "%c" {a..z} $'\n' >> ${result}
        printf "%c" {0..9} $'\n' >> ${result}

        # Put some delimiters in there to make sure they're interpreted correctly.
        printf "%s" {A..Z}, $'\n' >> ${result}
        printf "%s" {A..Z}$'\t' $'\n' >> ${result}

        # Put some special characters in there.
        printf "#@[]_!'{}()*&!$%%^&*()\n" >> ${result}

        # Add a very long line (note: it will be truncated)
        printf "%s" {A..Z}{A..Z} $'\n' >> ${result}

        # Read as text
        printf -v macro -- "-e '/fit{CE}%s~'" "${result}"
        macro+=$(printrange "${output}" A1..IV8192)
        macro+=$(quit)
        runmacro "${macro}"
        verifysum "${output}" 1980622935 10009
        endtest "${result}" "${output}"
    }
    starttest "/File Import Numbers" && {
        # Put some delimiters in there to make sure they're interpreted correctly.
        printf "%s" {0..255}, $'\n' >> ${result}
        printf "%s" {0..255}$'\t' $'\n' >> ${result}

        # Read as values
        printf -v macro -- "-e '/fin{CE}%s~'" "${result}"
        macro+=$(writerange "${output}" '@STRING(@SUM(A1..IV8192), 0)')
        macro+=$(quit)
        runmacro "${macro}"
        verifycontents "${output}" "65280"
        endtest "${result}" "${output}"
    }
    starttest "/Data Parse" && {
        macro=$(putstring A1 123X456Y789)
        macro+=$(putstring A2 321X654Y987)
        macro+=$(goto A1)
        macro+=$(sendkeys '/dpfcfeV>>SV>>SV>>~i{CE}A1..A3~o{R}~g')
        macro+=$(writerange "${result}" '@STRING(@SUM(B1..D2), 0)')
        macro+=$(quit)
        runmacro "${macro}"
        verifycontents "${result}" "3330"
        endtest "${result}"
    }
    starttest "/File Import Overwrite" && {
        printf "%s" {0..255}, $'\n' >> ${result}
        printf "%s" {1..255}, $'\n' >> ${result}
        printf "%s" {2..255}, $'\n' >> ${result}
        printf "%s" {3..255}, $'\n' >> ${result}
        printf "%s" {4..255}, $'\n' >> ${result}
        macro=$(sendkeys "/fin{CE}${result}~")
        macro+=$(goto A2)
        # repeat at different offset
        macro+=$(sendkeys "/fin{CE}${result}~")
        macro+=$(writerange "${output}" '@STRING(@SUM(A1..IV8192), 0)')
        macro+=$(quit)
        runmacro "${macro}"
        verifycontents "${output}" "195830"
        endtest "${result}" "${output}"
    }
    starttest "/File Import Numbers Mixed Labels/Values" && {
        printf '"foo",123,"bar","245"\n' > ${result}
        macro=$(sendkeys "/fin{CE}${result}~")
        macro+=$(writerange "${output}" '@STRING(@SUM(A1..IV8192), 0)')
        macro+=$(writerange "${result}" '@STRING(@VALUE(D1), 0)')
        macro+=$(quit)
        runmacro "${macro}"
        verifycontents "${output}" "123"
        verifycontents "${result}" "245"
        endtest "${result}" "${output}"
    }
    # Cleanup
    rm -rf "${output}" "${result}" "${lotdir}"
}

function start_stress_test()
{
    local scrdmp=$(mktemp -u)
    local result=$(mktemp -u)
    local import=$(mktemp -u)
    local macro
    local -i i=0

    starttest "Scroll Huge Sheet" && {
        local scrdmpa=$(mktemp -u)
        local scrdmpb=$(mktemp -u)
        local scrdmpc=$(mktemp -u)
        local scrdmpd=$(mktemp -u)
        # Generate a huge sheet of words
        egrep -i '^[a-z]+$' /usr/share/dict/words   \
            | sed -e 's/^/"/g' -e 's/$/", /g'       \
            | tr -d '\n'                            \
            | fold -s -w 511 > "${import}"
        # Verify it looks the same after scrolling.
        macro=$(noclock)
        macro+=$(import "${import}")
        macro+=$(sendkeys '{R 50}')
        macro+=$(sendkeys '{L 50}')
        macro+=$(screendump)
        macro+=$(saveas "${result}")
        macro+=$(quit)
        LOTUS_SCREEN_DUMP="${scrdmpa}" COLUMNS=380 LINES=72 runmacro "${macro}"
        macro=$(noclock)
        macro+=$(retrieve "${result}")
        macro+=$(sendkeys '{R 50}')
        macro+=$(sendkeys '{L 50}')
        macro+=$(screendump)
        macro+=$(quit)
        LOTUS_SCREEN_DUMP="${scrdmpb}" COLUMNS=380 LINES=72 runmacro "${macro}"
        macro=$(noclock)
        macro+=$(retrieve "${result}")
        macro+=$(sendkeys '{D 50}')
        macro+=$(sendkeys '{U 50}')
        macro+=$(screendump)
        macro+=$(quit)
        LOTUS_SCREEN_DUMP="${scrdmpc}" COLUMNS=380 LINES=72 runmacro "${macro}"
        macro=$(noclock)
        macro+=$(retrieve "${result}")
        macro+=$(sendkeys '{END}{HOME}')
        macro+=$(sendkeys '{HOME}')
        macro+=$(screendump)
        macro+=$(quit)
        LOTUS_SCREEN_DUMP="${scrdmpd}" COLUMNS=380 LINES=72 runmacro "${macro}"
        verifymatch "${scrdmpa}" "${scrdmpb}"
        verifymatch "${scrdmpa}" "${scrdmpc}"
        verifymatch "${scrdmpa}" "${scrdmpd}"
        endtest "${scrdmpa}" "${scrdmpb}" "${scrdmpc}" "${scrdmpd}" "${result}" "${import}"
    }
    starttest "@RAND Average" && {
        # Fill the sheet up with random numbers
        macro=$(sendkeys "@RAND~")
        macro+=$(sendkeys "/c~.{END}{R}{END}{D}~")
        # The average should be ~0.50, right?
        macro+=$(writerange "${result}" '@STRING(@AVG(A1..IV8192), 2)')
        macro+=$(quit)
        runmacro "${macro}"
        verifycontents "${result}" "0.50"
        endtest "${result}"
    }
    starttest "Huge Display" && {
        macro=$(noclock)
        macro+=$(sendkeys '/dfA1..IV8192~~~8192*8192~')
        macro+=$(screendump)
        macro+=$(quit)
        LOTUS_SCREEN_DUMP="${scrdmp}" COLUMNS=1000 LINES=500 runmacro "${macro}"
        verifyexist "${scrdmp}"
        verifysum "${scrdmp}" 3822333104 500500
        endtest "${scrdmp}"
    }
    starttest "Scroll Huge Diagonal Line" && {
        printf -v macro -- " -e '{{}R{}}{{}D{}}\\#{~}{{}RETURN{}}~{FOR A8192,1,255,1,A1..A1}'"
        LOTUS_SCREEN_DUMP="${scrdmp}" COLUMNS=1000 LINES=256 runmacro "${macro} $(screendump) $(quit)"

        verifyexist "${scrdmp}"
        sed 's/\s\+/ /g;255q;d' "${scrdmp}" > "${result}"
        verifycontents "${result}" "256 ######### \n"
        rm -f "${scrdmp}"

        # Now move it up, to test the screen drawing code.
        macro+=$(sendkeys '{U 256}')
        LOTUS_SCREEN_DUMP="${scrdmp}" COLUMNS=1000 LINES=256 runmacro "${macro} $(screendump) $(quit)"
        verifyexist "${scrdmp}"
        sed 's/\s\+/ /g;255q;d' "${scrdmp}" > "${result}"
        verifycontents "${result}" "251 ######### \n"
        rm -f "${scrdmp}"

        endtest "${scrdmp}" "${result}"
    }
    # This was tricky to get right, make sure it works.
    starttest "Hilight Full Range From Prev Sheet" && {
        macro=$(noclock)
        macro+=$(sendkeys '/dfA1..IV8192~~~8192*8192~/wis~~')
        macro+=$(sendkeys '/df')
        macro+=$(screendump)
        macro+=$(sendkeys '{ESC 5}')
        macro+=$(quit)
        LOTUS_SCREEN_DUMP="${scrdmp}" COLUMNS=1000 LINES=100 runmacro "${macro}"
        verifysum "${scrdmp}" 3606002881 100100
        endtest "${scrdmp}"
    }
    starttest "Split Large Window" && {
        macro=$(goto GG1)
        macro+=$(sendkeys '/wwv')
        macro+=$(sendkeys '/wwc')
        macro+=$(goto GG1024)
        macro+=$(sendkeys '/wwh')
        macro+=$(sendkeys '/wwc')
        macro+=$(quit)
        COLUMNS=8192 LINES=1024 runmacro "${macro}"
        endtest
    }
    starttest "Render Huuuuuge Map" && {
        macro=$(noclock)
        macro+=$(sendkeys "/dfA1..IV8192~~~8192*8192~")
        macro+=$(sendkeys "/wwme")
        macro+=$(screendump)
        macro+=$(sendkeys "{ESC 5}")
        macro+=$(quit)
        LOTUS_SCREEN_DUMP="${scrdmp}" COLUMNS=1000 LINES=1024 runmacro "${macro}"
        verifysum "${scrdmp}" 2846770937 1025024
        endtest "${scrdmp}"
    }
    starttest "Huge 3D-Range" && {
        # Fill the sheet up with junk
        macro=$(sendkeys "/wisa25~")
        macro+=$(sendkeys "/df{CE}A:A1..Z:IV8192~~~8192*256*26~")
        # Now the 3d-range A:A1..Z:IV8192 is truly enormous
        macro+=$(writerange "${result}" '@STRING(@SUM(A:A1..Z:IV8192), 0)')
        macro+=$(quit)
        runmacro "${macro}"
        verifycontents "${result}" "1486539693490180"
        endtest "${result}"
    }
    starttest "Full Perspective Sheets" && {
        macro=$(noclock)
        # Fill the sheet up with junk
        for sheet in {A..C}; do
            macro+=$(sendkeys "/df{CE}${sheet}:A1..${sheet}:IV8192~${i}~~8192*256+${i}~")
            macro+=$(sendkeys "/wisa~")
            let i++
        done
        macro+=$(sendkeys "/wwp")
        macro+=$(sendkeys "{PS 3}")
        macro+=$(screendump)
        macro+=$(quit)
        LOTUS_SCREEN_DUMP="${scrdmp}" COLUMNS=100 LINES=50 runmacro "${macro}"
        verifysum "${scrdmp}" 2002276367 5050
        endtest "${scrdmp}"
    }
}

function show_help()
{
    printf "usage: %s [cmd]\n" "${1}"
    printf "    test    - verify that outputs still match golden outputs\n"
    printf "    calc    - verify the result of some calculations\n"
    printf "    menu    - verify various menu options are working\n"
    printf "    file    - check that file management works\n"
    printf "    stress  - generate lots of data and check results\n"
    printf "    gold    - generate golden outputs\n"
    printf "    help    - print this message\n"
    printf "    all     - run all available tests\n"
    printf "\n"
    printf "Your terminal may flicker or look frozen as the test macros play.\n"
    exit 1
}

function check_menu_options()
{
    local scrdmp=$(mktemp -u)
    local result=$(mktemp -u)

    starttest "Signal Macro" && {
        macro=$(sendkeys '{{}OPEN "'"${result}"'","w"{}}~{D}^/qyy~')
        macro+=$(sendkeys '/rnc{CE}\1~{HOME}~')
        macro+=$(system "kill -USR2 \${PPID}")
        runmacro "${macro}"
        verifyexist "${result}"
        endtest "${result}"
    }

    starttest "Move Range Across Vertical Window" && {
        macro=$(noclock)
        macro+=$(sendkeys '/dfA1..IV8192~~~256*8192~/wis~~')
        macro+=$(goto B:AF1024)
        macro+=$(sendkeys '{R 40}')
        macro+=$(sendkeys '/wwv')
        macro+=$(sendkeys '{PS}{WINDOW}{R 5}{D 5}')
        macro+=$(sendkeys '/m.{R 10}{D 10}~{WINDOW}{L 20}{D 5}~')
        macro+=$(screendump)
        macro+=$(quit)
        LOTUS_SCREEN_DUMP="${scrdmp}" COLUMNS=1000 LINES=100 runmacro "${macro}"
        verifysum "${scrdmp}" 1287976195 100100
        endtest "${scrdmp}"
    }
    return 0;
}

# Code numbers for [cell-format]:
# 0 - 15   Fixed, 0-15 decimals         117  Text
# 16 - 31  Scientific, 0-15 decimals    118  Hidden
# 32 - 47  Currency, 0-15 decimals      119  D6 (HH:MM:SS AM/PM)
# 48 - 63  Percent, 0-15 decimals       120  D7 (HH:MM AM/PM)
# 64 - 79  Comma, 0-15 decimals         121  D4 (Long Intn'l Date)
# 112      +/-                          122  D5 (Short Intn'l Date)
# 113      General                      123  D8 (Long Intn'l Time)
# 114      D1 (DD-MMM-YY)               124  D9 (Short Intn'l Time)
# 115      D2 (DD-MMM)                  127  Worksheet's global cell
# 116      D3 (MMM-YY)                       format
function check_calculations()
{
    # values
    verify_result_contents "1" "1"
    verify_result_contents "1+1" "2"
    verify_result_contents '+"hello" & "world"' "helloworld"

    # labels
    verify_result_contents "^abc" "abc"
    verify_result_contents '"def' "def"

    # functions
    verify_result_contents "@ABS(-123)" "123"
    verify_result_contents "@ACOS(0.123)" "1.447484052"
    verify_result_contents "@ASIN(0.123)" "0.123312275"
    verify_result_contents "@ATAN(0.123)" "0.122385281"
    verify_result_contents "@ATAN2(0.123,0.456)" "1.307329786"
    verify_result_contents "@AVG(123,456)" "289.5"
    verify_result_contents '@CELL("col",X123)' "24"
    verify_result_contents '@CELLPOINTER("row")' "1"
    verify_result_contents "@CHAR(65)" "A"
    verify_result_contents '@CHOOSE(2,"foo","bar","baz","quux")' "baz"
    verify_result_contents '@CODE("A")' "65"
    verify_result_contents "@COLS(B1..D1)" "3"
    verify_result_contents "@COORD(2,3,4,1)" '$B:$C$4'
    verify_result_contents "@COS(90)" "-0.44807362"
    verify_result_contents "@COUNT(A1..Z10)" "1"
    verify_result_contents "@CTERM(0.05,10000,800)" "51.76716839"

    # Check date handling
    verify_result_contents "@DATE(00,1,1)" "01-Jan-00" 114
    verify_result_contents "@DATE(2000,1,1)" "01-Jan-2000" 114
    # Verify leap years are handled correctly
    verify_result_contents "@DATE(2020,2,29)" "29-Feb-2020" 114
    verify_result_contents "@DATE(2021,2,29)" "ERR" 114
    verify_result_contents "@DATE(2024,2,29)" "29-Feb-2024" 114
    verify_result_contents "@DATE(2022,2,28)+1" "01-Mar-2022" 114
    # Check for 2038 problems
    verify_result_contents "@DATE(2038,1,20)" "20-Jan-2038" 114

    verify_result_contents '@DATEVALUE("5/23/2022")' "23-May-2022" 114
    verify_result_contents "@DAY(@DATE(2024,2,29))" "29"

    verify_result_contents "@DDB(12345,100,10,1)" "2469"
    verify_result_contents "@D360(@DATE(2022,1,1),@DATE(2022,2,1))" "30"
    verify_result_contents "@ERR" "ERR"
    verify_result_contents '@EXACT("ABC","DEF")' "0"
    verify_result_contents '@EXACT("ABC","ABC")' "1"
    verify_result_contents "@EXP(123)" "2.6195E+53"
    verify_result_contents "@FALSE" "0"
    verify_result_contents '@FIND("lo","hello world",0)' "3"
    verify_result_contents "@FV(123,0.08,100)" "3380595.432"
    verify_result_contents "@HOUR(0.9999)" "23"
    verify_result_contents "@HOUR(0)" "0"
    verify_result_contents "@IF(@FALSE,1,2)" "2"
    verify_result_contents "@IF(@TRUE,1,2)" "1"
    verify_result_contents '@INFO("system")' "systemv"
    verify_result_contents '{SYSTEM "exit 123"}@INFO("osreturncode")/2^8' "123"
    verify_result_contents "@INT(7/3)" "2"
    verify_result_contents "@ISERR(@ERR)" "1"
    verify_result_contents "@ISERR(@TRUE)" "0"
    verify_result_contents "@ISNA(@ERR)" "0"
    verify_result_contents "@ISNA(@NA)" "1"
    verify_result_contents '@LEFT("hello",3)' "hel"
    verify_result_contents '@LENGTH("hello")' "5"
    verify_result_contents "@LN(123)" "4.812184355"
    verify_result_contents "@LOG(123)" "2.089905111"
    verify_result_contents '@LOWER("cRaZy")' "crazy"
    verify_result_contents "@MAX(12,88,3,54,89,2,1.3,-4)" "89"
    verify_result_contents '@MID("hello world",6,2)' "wo"
    verify_result_contents "@MIN(12,88,3,54,89,2,1.3,-4)" "-4"
    verify_result_contents "@MOD(9,4)" "1"
    verify_result_contents "@PI" "3.141592653589790" 15 20
    verify_result_contents "@PMT(1000,0.08,12)" "132.6950169"
    verify_result_contents '@PROPER("hello world")' "Hello World"
    verify_result_contents "@PV(100,0.02,12)" "1057.534122"
    verify_result_contents "@RATE(1000,123,30)" "0.072349852"
    verify_result_contents '@REPEAT("abc",3)' "abcabcabc"
    verify_result_contents '@REPLACE("hello",2,2,"ZZ")' "heZZo"
    verify_result_contents '@RIGHT("hello", 2)' "lo"
    verify_result_contents "@ROUND(@PI, 1)" "3.1"
    verify_result_contents "@SIN(123)" "-0.45990349"
    verify_result_contents "@SQRT(2)" "1.414213562"
    verify_result_contents "@TAN(2)" "-2.18503986"
    verify_result_contents '@TRIM(" h  e  l     l o ")' "h e l l o"
    verify_result_contents '@UPPER("hello")' "HELLO"
}

function run_all_tests()
{
    verify_output_matches
    check_calculations
    check_menu_options
    verify_file_ops
    start_stress_test
    exit 0
}

case "${1}" in
    all) run_all_tests;;
    gold) generate_gold_output;;
    test) verify_output_matches;;
    calc) check_calculations;;
    menu) check_menu_options;;
    file) verify_file_ops;;
    stress) start_stress_test;;
    *) show_help "${0##*/}" >&2;;
esac

exit 0
