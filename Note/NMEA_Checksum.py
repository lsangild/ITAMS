#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Apr 12 10:50:12 2018

@author: lsangild
"""

buffer = b'PUBX,40,RMC,0,1,0,0,0,0'
checksum = 0x00;

for i in range(len(buffer)):
    checksum = checksum ^ buffer[i]
    
print(hex(checksum))