make -B

if [ $? -eq 0 ]
then
  for stage in stage1 stage2;
  do
    ./modmul $stage < $stage.input 2>/dev/null | cmp $stage.output -s -

    if [ $? -eq 0 ]
    then
      echo success $stage
    else
      echo failure $stage

      exit 1
    fi
  done
fi

exit 0
