#!/bin/bash
for tsfile in *.ts ; do
	lupdate-qt5 .. -ts "$tsfile"
done

