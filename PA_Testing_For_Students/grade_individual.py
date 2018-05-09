# Michael Fulton
# Inspiration and help from Christopher Jonathan and Louai Alarabi
import os
import sys
import subprocess
import shutil
import filecmp

def checkTestCase(case):
  points = 5
  err  = list()
  err.append("  [")
  if case == 'AV_Test':
    os.chdir(case)
    sfile = open("messages_sol.txt", "r")
    rfile = open("messages.txt", "r")

    for sline in sfile:
      sline.strip('\n')
      smode, scode, sregion, sdata = parseReqRes(sline)

      rline = rfile.readline().strip('\n')
      rmode, rcode, rregion, rdata = parseReqRes(rline)
  
      if smode == 'END':
        break

      if scode == rcode:
        continue
      else:
        err.append("Sol: {0} =/= {1}, ".format(scode, rcode))
        points -= 1
        
    err.append("]")

    f = os.path.abspath("messages.txt")
    os.chdir("..")
    if points < 0 :
      return 0, ''.join(err), "File: {0}".format(f)
    else:
      return points, ''.join(err), "File: {0}".format(f)


  elif case == 'CP_Test':
    os.chdir(case)
    sfile = open("messages_sol.txt", "r")
    rfile = open("messages.txt", "r")

    for sline in sfile:
      sline.strip('\n')
      smode, scode, sregion, sdata = parseReqRes(sline)

      rline = rfile.readline().strip('\n')
      rmode, rcode, rregion, rdata = parseReqRes(rline)
  
      if smode == 'END':
        break

      if scode == rcode:
        continue
      else:
        err.append("Sol: {0} =/= {1}, ".format(scode, rcode))
        points -= 1
        
    err.append("]")

    f = os.path.abspath("messages.txt")
    os.chdir("..")
    if points < 0 :
      return 0, ''.join(err), "File: {0}".format(f)
    else:
      return points, ''.join(err), "File: {0}".format(f)


  elif case == 'CV_Test':
    os.chdir(case)
    sfile = open("messages_sol.txt", "r")
    rfile = open("messages.txt", "r")

    for sline in sfile:
      sline.strip('\n')
      smode, scode, sregion, sdata = parseReqRes(sline)

      rline = rfile.readline().strip('\n')
      rmode, rcode, rregion, rdata = parseReqRes(rline)
  
      if smode == 'END':
        break

      if scode == rcode:
        continue
      else:
        err.append("Sol: {0} =/= {1}, ".format(scode, rcode))
        points -= 1
        
    err.append("]")

    f = os.path.abspath("messages.txt")
    os.chdir("..")
    if points < 0 :
      return 0, ''.join(err), "File: {0}".format(f)
    else:
      return points, ''.join(err), "File: {0}".format(f)


  elif case == 'OP_Test':
    os.chdir(case)
    sfile = open("messages_sol.txt", "r")
    rfile = open("messages.txt", "r")

    for sline in sfile:
      sline.strip('\n')
      smode, scode, sregion, sdata = parseReqRes(sline)

      rline = rfile.readline().strip('\n')
      rmode, rcode, rregion, rdata = parseReqRes(rline)
  
      if smode == 'END':
        break

      if scode == rcode:
        continue
      else:
        err.append("Sol: {0} =/= {1}, ".format(scode, rcode))
        points -= 1
        
    err.append("]")

    f = os.path.abspath("messages.txt")
    os.chdir("..")
    if points < 0 :
      return 0, ''.join(err), "File: {0}".format(f)
    else:
      return points, ''.join(err), "File: {0}".format(f)


  elif case == 'RW_Test':
    os.chdir(case)
    sfile = open("messages1_sol.txt", "r")
    rfile = open("messages1.txt", "r")

    for sline in sfile:
      sline.strip('\n')
      smode, scode, sregion, sdata = parseReqRes(sline)

      rline = rfile.readline().strip('\n')
      rmode, rcode, rregion, rdata = parseReqRes(rline)
  
      if smode == 'END':
        break

      if scode == rcode:
        continue
      else:
        if scode == 'RO' or scode == 'RC':
          if rcode == 'RC' or rcode == 'RO':
            continue
        else:
          err.append("Sol: {0} =/= {1}, ".format(scode, rcode))
          points -= 1
        
    err.append("]")
    f = os.path.abspath("messages.txt")
    os.chdir("..")
    if points < 0 :
      return 0, ''.join(err), "File: {0}".format(f)
    else:
      return points, ''.join(err), "File: {0}".format(f)


  elif case == 'RV_Test':
    os.chdir(case)
    sfile = open("messages_sol.txt", "r")
    rfile = open("messages.txt", "r")

    for sline in sfile:
      sline.strip('\n')
      smode, scode, sregion, sdata = parseReqRes(sline)

      rline = rfile.readline().strip('\n')
      rmode, rcode, rregion, rdata = parseReqRes(rline)
  
      if smode == 'END':
        break

      if scode == rcode:
        continue
      else:
        err.append("Sol: {0} =/= {1}, ".format(scode, rcode))
        points -= 1
        
    err.append("]")

    f = os.path.abspath("messages.txt")
    os.chdir("..")
    if points < 0 :
      return 0, ''.join(err), "File: {0}".format(f)
    else:
      return points, ''.join(err), "File: {0}".format(f)


  elif case == 'TestCase01':
    os.chdir(case)
    sfile = open("messages_sol.txt", "r")
    rfile = open("messages.txt", "r")

    for sline in sfile:
      sline.strip('\n')
      smode, scode, sregion, sdata = parseReqRes(sline)

      rline = rfile.readline().strip('\n')
      rmode, rcode, rregion, rdata = parseReqRes(rline)

      if smode == 'END':
        break

      if scode == rcode:
        continue
      else:
        err.append("Sol: {0} =/= {1}, ".format(scode, rcode))
        points -= 1

    err.append("]")

    f = os.path.abspath("messages.txt")
    os.chdir("..")
    if points < 0 :
      return 0, ''.join(err), "File: {0}".format(f)
    else:
      return points, ''.join(err), "File: {0}".format(f)


  elif case == 'TestCase02':
    os.chdir(case)
    sfile = open("messages_sol.txt", "r")
    rfile = open("messages.txt", "r")

    for sline in sfile:
      sline.strip('\n')
      smode, scode, sregion, sdata = parseReqRes(sline)

      rline = rfile.readline().strip('\n')
      rmode, rcode, rregion, rdata = parseReqRes(rline)

      if smode == 'END':
        break

      if scode == rcode:
        continue
      else:
        err.append("Sol: {0} =/= {1}, ".format(scode, rcode))
        points -= 1

    err.append("]")

    f = os.path.abspath("messages.txt")
    os.chdir("..")
    if points < 0 :
      return 0, ''.join(err), "File: {0}".format(f)
    else:
      return points, ''.join(err), "File: {0}".format(f)


  elif case == 'TestCase03':
    os.chdir(case)
    sfile = open("messages_sol.txt", "r")
    rfile = open("messages.txt", "r")

    for sline in sfile:
      sline.strip('\n')
      smode, scode, sregion, sdata = parseReqRes(sline)

      rline = rfile.readline().strip('\n')
      rmode, rcode, rregion, rdata = parseReqRes(rline)

      if smode == 'END':
        break

      if scode == rcode:
        continue
      else:
        err.append("Sol: {0} =/= {1}, ".format(scode, rcode))
        points -= 1

    err.append("]")

    f = os.path.abspath("messages.txt")
    os.chdir("..")
    if points < 0 :
      return 0, ''.join(err), "File: {0}".format(f)
    else:
      return points, ''.join(err), "File: {0}".format(f)


  elif case == 'TestCase04':
    os.chdir(case)
    sfile = open("messages_sol.txt", "r")
    rfile = open("messages.txt", "r")

    for sline in sfile:
      sline.strip('\n')
      smode, scode, sregion, sdata = parseReqRes(sline)

      rline = rfile.readline().strip('\n')
      rmode, rcode, rregion, rdata = parseReqRes(rline)

      if smode == 'END':
        break

      if scode == rcode:
        continue
      else:
        err.append("Sol: {0} =/= {1}, ".format(scode, rcode))
        points -= 1

    err.append("]")

    f = os.path.abspath("messages.txt")
    os.chdir("..")
    if points < 0 :
      return 0, ''.join(err), "File: {0}".format(f)
    else:
      return points, ''.join(err), "File: {0}".format(f)


  elif case == 'TestCase05':
    return -1

  elif case == 'TestCase06':
    return -1

  elif case == 'TestCase07':
    return -1

  elif case == 'TestCaseE1':
    return -1

  elif case == 'TestCaseE2':
    return -1

  else:
    return -1

def parseReqRes(line):
  res = line.split('\'')
  if(len(res) == 1):
    mode = 'END'
    return mode, '', '', ''

  mode = res[0].strip()
  msg = res[1]
  res = msg.split(';')

  code = res[0]
  region = ''
  data = ''
  if mode == 'REQ:':
    region = res[1].strip()
    data = res[2].strip('\\x00')
  elif mode == 'RESP:':
    data = res[1].strip('\\x00')

  return mode, code, region, data

if __name__ == '__main__':

  if len(sys.argv)  < 2:
    print "Not enough arguments"
    exit(1)
  elif len(sys.argv) < 3:
    folder = sys.argv[1]
    mode = 'run'
  else:
    folder = sys.argv[1]
    mode = sys.argv[2]

  print folder, mode
  os.chdir(os.path.abspath(folder))

  if mode == 'runcheck' or mode == 'run':
    print("Running test cases for " + folder)
    os.system("./move_executables.bash")
    os.system("./run_all.bash 4061")

  print("Checking test cases for " + folder)

  output = open("GRADE.txt", 'w')
  print("Output in GRADE.txt") 

  # Test if readme exists
  if os.path.exists('README') or os.path.exists('README.txt') or os.path.exists('readme.txt') or os.path.exists('readme'):
    output.write("Readme appears to exist.\n")
  else:
    output.write("Readme does not appear to exist.\n")
  
  output.write('\n')

  # Test if makefile existence.
  if os.path.exists('Makefile') or os.path.exists('makefile'):
    output.write('Makefile appears to exist\n')
  else:
    output.write('Makefile does not appear to exist.\n')
    exit(1)

  output.write('\n')

  #Check server and client existence.
  if os.path.exists('server'):
    output.write('Server appears to exist and ')
  else:
    output.write('Server does not appear to exist ')

  if os.path.exists('client'):
    output.write('client appears to exist.\n')
  else:
    output.write('client does not appear to exist.\n')
    exit(1)

  output.write('\n')

  # Check client and server with nonexistant files.
  output.write('Server output when called with empty file [')
  output.write(open('nonexistant_serv.out', 'r').read().strip('\n'))
  output.write(']\n')
  output.write('Client output when called without server [')
  output.write(open('nonexistant_client.out', 'r').read().strip('\n'))
  output.write(']\n')
  output.write('\n')

  #Create messages dump
  dump = open('messages_dump.txt', 'w')
  sdump = open('messages_sol_dump.txt', 'w')

  dump.write('------------------AV_TEST-------------\n')
  sdump.write('-----------------AV_TEST-------------\n')
  
  with open('AV_Test/messages.txt', 'r') as f:
    dump.write(f.read())
  with open('AV_Test/messages_sol.txt', 'r') as f:
    sdump.write(f.read())

  dump.write('------------------CP_TEST-------------\n')
  sdump.write('-----------------CP_TEST-------------\n')

  with open('CP_Test/messages.txt', 'r') as f:
    dump.write(f.read())
  with open('CP_Test/messages_sol.txt', 'r') as f:
    sdump.write(f.read())

  dump.write('------------------CV_TEST-------------\n')
  sdump.write('-----------------CV_TEST-------------\n')

  with open('CV_Test/messages.txt', 'r') as f:
    dump.write(f.read())
  with open('CV_Test/messages_sol.txt', 'r') as f:
    sdump.write(f.read())
  dump.write('------------------OP_TEST-------------\n')
  sdump.write('-----------------OP_TEST-------------\n')


  with open('OP_Test/messages.txt', 'r') as f:
    dump.write(f.read())
  with open('OP_Test/messages_sol.txt', 'r') as f:
    sdump.write(f.read())
  dump.write('------------------RV_TEST-------------\n')
  sdump.write('-----------------RV_TEST-------------\n')


  with open('RV_Test/messages.txt', 'r') as f:
    dump.write(f.read())
  with open('RV_Test/messages_sol.txt', 'r') as f:
    sdump.write(f.read())

  dump.write('------------------RW_TEST1-------------\n')
  sdump.write('-----------------RW_TEST1-------------\n')


  with open('RW_Test/messages1.txt', 'r') as f:
    dump.write(f.read())
  with open('RW_Test/messages1_sol.txt', 'r') as f:
    sdump.write(f.read())

  dump.write('------------------RW_TEST2-------------\n')
  sdump.write('-----------------RW_TEST2-------------\n')

  with open('RW_Test/messages2.txt', 'r') as f:
    dump.write(f.read())
  with open('RW_Test/messages2_sol.txt', 'r') as f:
    sdump.write(f.read())


  dump.write('------------------TestCase01-------------\n')
  sdump.write('-----------------TestCase01-------------\n')

  with open('TestCase01/messages.txt', 'r') as f:
    dump.write(f.read())
  with open('TestCase01/messages_sol.txt', 'r') as f:
    sdump.write(f.read())

  dump.write('------------------TestCase02-------------\n')
  sdump.write('-----------------TestCase02-------------\n')

  with open('TestCase02/messages.txt', 'r') as f:
    dump.write(f.read())
  with open('TestCase02/messages_sol.txt', 'r') as f:
    sdump.write(f.read())

  dump.write('------------------TestCase03-------------\n')
  sdump.write('-----------------TestCase03-------------\n')

  with open('TestCase03/messages.txt', 'r') as f:
    dump.write(f.read())
  with open('TestCase03/messages_sol.txt', 'r') as f:
    sdump.write(f.read())

  dump.write('------------------TestCase04-------------\n')
  sdump.write('-----------------TestCase04-------------\n')

  with open('TestCase04/messages.txt', 'r') as f:
    dump.write(f.read())
  with open('TestCase04/messages_sol.txt', 'r') as f:
    sdump.write(f.read())

  dump.write('------------------TestCase05-------------\n')
  sdump.write('-----------------TestCase05-------------\n')

  try:
    f = open('TestCase05/messages4.txt', 'r') 
    dump.write(f.read())
  except IOError:
    dump.write('File not found.')

  with open('TestCase05/messages4_sol.txt', 'r') as f:
    sdump.write(f.read())

  dump.write('------------------TestCase06-------------\n')
  sdump.write('-----------------TestCase06-------------\n')

  try:
    f = open('TestCase06/messages5.txt', 'r')
    dump.write(f.read())
  except IOError:
    dump.write('File not found.')

  with open('TestCase06/messages5_sol.txt', 'r') as f:
    sdump.write(f.read())


  dump.write('------------------TestCase07-------------\n')
  sdump.write('-----------------TestCase07-------------\n')
  
  try:
    f =open('TestCase07/messages7.txt', 'r')
    dump.write(f.read())
  except IOError:
    dump.write('File not found.')

  with open('TestCase07/messages7_sol.txt', 'r') as f:
    sdump.write(f.read())


  dump.write('------------------TestCaseE1-------------\n')
  sdump.write('-----------------TestCaseE1-------------\n')
  
  try:
    f =open('TestCaseE1/messages.txt', 'r')
    dump.write(f.read())
  except IOError:
    dump.write('File not found.')

  with open('TestCaseE1/messages_sol.txt', 'r') as f:
    sdump.write(f.read())

  dump.write('------------------TestCaseE2-------------\n')
  sdump.write('-----------------TestCaseE2-------------\n')
  
  try:
    f =open('TestCaseE2/messages2.txt', 'r')
    dump.write(f.read())
  except IOError:
    dump.write('File not found.')

  with open('TestCaseE2/messages_sol.txt', 'r') as f:
    sdump.write(f.read())

  
  #Check command tests
  output.write('--------------------Command Test Cases----------------------\n')

  output.write('AV_Test: '+ str(checkTestCase('AV_Test')) + '\n')
  output.write('CP_Test: '+ str(checkTestCase('CP_Test')) + '\n')
  output.write('CV_Test: '+ str(checkTestCase('CV_Test')) + '\n')
  output.write('OP_Test: '+ str(checkTestCase('OP_Test')) + '\n')
  output.write('RV_Test: '+ str(checkTestCase('RV_Test')) + '\n')
  output.write('RW_Test: '+ str(checkTestCase('RW_Test')) + '\n')
  
  #Check single client test cases
  output.write('-------------------Single Client Test Cases------------------\n')

  output.write('TestCase01: '+ str(checkTestCase('TestCase01')) + '\n')
  output.write('TestCase02: '+ str(checkTestCase('TestCase02')) + '\n')
  output.write('TestCase03: '+ str(checkTestCase('TestCase03')) + '\n')
  output.write('TestCase04: '+ str(checkTestCase('TestCase04')) + '\n')

  #Check multi-client test cases
  output.write('------------------Multi-Client Test Cases--------------------\n')


  output.write('TestCase05: '+ str(checkTestCase('TestCase05')) + '\n')
  output.write('TestCase06: '+ str(checkTestCase('TestCase06')) + '\n')
  output.write('TestCase07: '+ str(checkTestCase('TestCase07')) + '\n')

  #Check extra credit test cases
  output.write('-----------------Extra Credit Test Cases---------------------\n')

  output.write('TestCaseE1: '+ str(checkTestCase('TestCaseE1')) + '\n')
  output.write('TestCaseE2: '+ str(checkTestCase('TestCaseE2')) + '\n')
