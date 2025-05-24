#!/usr/bin/env bash
/usr/bin/socat TCP4-LISTEN:31242,reuseaddr,fork EXEC:"/usr/bin/wine /home/user/app.exe"