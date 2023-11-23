#!/bin/bash

# Get all experiment IDs
experiments=$(iotlab-experiment get -e)
ids=$(echo $experiments | jq -r '.Running[]')

# Stop all experiments
echo "Stopping all experiments"
for id in $ids; do
    iotlab-experiment stop -i ${id}
done
