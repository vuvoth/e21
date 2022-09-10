#!/bin/bash

number_user=20

echo "Benchmark for merkle tree deep = 8 => 2^8 node!!!"
echo -e
for tx_per_epoch in {10..50..5}
do
    echo "=========================================="
    echo "Benckmark with tx/epoch = $tx_per_epoch"
    ./zkrle21 init $number_user $tx_per_epoch | grep "Total constraints"
    ./zkrle21 deposit $tx_per_epoch | grep "Gas code for rollup"
    ./zkrle21 transfer $tx_per_epoch | grep "Gas code for rollup"
    ./zkrle21 withdraw $tx_per_epoch | grep "Gas code for rollup"
    echo "=========================================="
    echo -e
done

