#!/usr/bin/env python
import argparse
import logging
import os
import statistics
import sys
import yaml

import matplotlib.pyplot as plt

def get_median(l):
  assert isinstance(l, list)
  assert len(l) > 1

  # Drop the first result, it is a warm up run
  return statistics.median(l[1:])

def main(args):
  parser = argparse.ArgumentParser(description='Plot results')
  parser.add_argument('result_yml', type=argparse.FileType('r'))
  args = parser.parse_args(args)

  rawData = yaml.load(args.result_yml, Loader=yaml.CLoader)
  dimSizes = []
  vector_half = []
  vector_single = []
  soft_half = []

  # Process the data
  for d in rawData:
    assert isinstance(d, dict)
    dimSizes.append(d['dim_size'])
    vector_half.append(get_median(d['vector_f16_f32_f16']))
    vector_single.append(get_median(d['vector_f32']))
    soft_half.append(get_median(d['soft_f16_f32_f16']))

  # plot data
  lines = [
    { 'title':'vector_half', 'data':vector_half},
    { 'title':'vector_single', 'data':vector_single},
    { 'title':'soft_half', 'data':soft_half},
  ]

  fig, ax = plt.subplots()
  ax.set_xlabel('Image size (# of pixels)', fontsize=22)
  ax.set_ylabel('Time ($\mu$s)', fontsize=22)

  curves = []
  legendNames = []
  for l in lines:
    p = ax.plot(dimSizes, l['data'])
    curves.append(p[0])
    legendNames.append(l['title'])

  legend = ax.legend(tuple(curves), tuple(legendNames), loc='upper left')

  plt.show()

if __name__ == '__main__':
  main(sys.argv[1:])
