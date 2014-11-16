#!/bin/bash
cd "$(dirname "$0")"
./facerecognition -s >> log/server.log&