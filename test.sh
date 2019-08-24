RES=$(make test) && [ $(echo "$RES" | grep "OK" | wc -l) == "7" ] && echo "All C/L/C tests passed" && exit 0

echo $RES

echo "Failed!"
exit 1
