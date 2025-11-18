# These are only the executing instruction records not the script itself.
# ==========================================================================
for i in $(seq 1 20); do
  python3 agents/main_batch.py data_input.json -o .log/run_$i --batch_size 32
done
# ==========================================================================
for i in $(seq 1 20); do
  python checker.py \
    --root .log/run_$i \
    --json data_output/results$i.json \
    --csv data_output/summary$i.csv
done
