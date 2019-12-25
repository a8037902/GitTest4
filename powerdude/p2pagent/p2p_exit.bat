@echo off
REM echo "Exit p2p-relay"
tasklist | find /i "p2p-relay.exe" && taskkill /f /im  p2p-relay.exe

REM echo "Exit openvpn"
tasklist | find /i "openvpn.exe" && taskkill /f /im  openvpn.exe

REM echo "Exit p2pagent"
tasklist | find /i "p2pagent.exe" && taskkill /f /im  p2pagent.exe

echo "Exit p2p over."