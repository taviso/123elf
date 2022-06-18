#!/bin/bash

# This file contains helper functions for building test macros.

# printrange file range
function printrange()
{
    printf -- " -e '/pf{CE}%s~r{CE}%s~gq'" "${1}" "${2}"
}

function quit()
{
    printf -- " -e '/qyy'"
}

# escape [count]
function escape()
{
    printf -- " -e '{ESC %s}'" "${1:-1}"
}

function enter()
{
    printf -- " -e '~'"
}

# goto [cell]
function goto()
{
    printf -- " -e '{GOTO}{CE}%s~'" "${1:-A1}"
}

# sendkeys string
function sendkeys()
{
    printf -- " -e '%s'" "${@}"
}

# putstring cell string
function putstring()
{
    printf -- " -e '{PUT %s,0,0,%s}'" "${1}" "${2}"
}

# saveas filename
function saveas()
{
    printf -- " -e '/fs{CE}%s~'" "${1}"
}

# retrieve filename
function retrieve()
{
    printf -- " -e '/fr{CE}%s~'" "${1}"
}

# system shellcommand
function system()
{
    printf -- " -e '{SYSTEM %c%s%c}'" '"' "${*}" '"'
}

# macsleep seconds
function macsleep()
{
    printf -- " -e '{WAIT @NOW+@TIME(0,0,%u)}'" "${1:-1}"
}

function screendump()
{
    printf -- " -e '{WINDOWSOFF}{SYSTEM +%s & @CHAR(36) & %s}{WINDOWSON}'" '"kill -USR1 "' '"{PPID}"'
}

# writerange file range
function writerange()
{
    local o w c
    printf -v o -- '{OPEN "%s","w"}' "${1}"
    printf -v w -- '{WRITE %s}' "${2}"
    printf -v c -- '{CLOSE}'
    printf -- " -e '%s'" "${o}" "${w}" "${c}"
}

# This turns off the clock -- useful to make screendumps deterministic.
function noclock()
{
    printf -- " -e '/wgdocnq'"
}
