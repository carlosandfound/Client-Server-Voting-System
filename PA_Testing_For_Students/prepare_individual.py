# Michael Fulton
# Inspiration and help from Christopher Jonathan and Louai Alarabi
import os
import zipfile
import sys
import subprocess
import shutil
import filecmp
from tqdm import tqdm


#Make every makefile from the root.
def make(student_folder):
    os.chdir(os.path.abspath(student_folder)) #Change to the new working directory.
    os.system('make clean > temp.txt 2> temp_err.txt')
    os.system('rm temp.txt temp_err.txt')
    os.system('make > make_result.txt 2> temp_err.txt')
    return


#Copy test cases to the submission_dir
def copy_test_cases(student_folder, test_cases):
    student = os.path.abspath(student_folder)
    for test_case in test_cases:
        if os.path.exists(os.path.join(student,test_case)):
            os.system("rm -rf " + os.path.join(student,test_case))
            
        os.system("cp -r "+test_case+" "+student)
        os.system("cp move_executables.bash " +student)
        os.system("cp run_all.bash " +student)
    return

##########################################
# MAIN
##########################################

if len(sys.argv) != 2:
    print("Usage: ./prepare_failed.py <submission_zip> <output_directory>\n")
    sys.exit(0)

#Parse the arguments.
student_dir = sys.argv[1]
root_dir = os.getcwd()
test_case = ["AV_Test", "CP_Test", "CV_Test", "OP_Test", "RV_Test", "RW_Test", "TestCase01", "TestCase02", "TestCase03", "TestCase04", "TestCase05", "TestCase06", "TestCase07", "TestCaseE1", "TestCaseE2"]


#Remove memory map error printing.
os.system("export MALLOC_CHECK_=0")


#Make all submissions.
print("STAGE 2: Make students' submissions....")
make(os.path.abspath(student_dir))
os.chdir(root_dir)
print("DONE!\n")

#Copying test cases to each student folder.
print("STAGE 3: Copying test cases to each student folder....")
copy_test_cases(os.path.abspath(student_dir), test_case)
print("DONE!\n")