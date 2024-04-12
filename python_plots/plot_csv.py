import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import time
from natsort import natsorted
import openpyxl as xl

global x_start_e, x_stop_e, x_start_d, x_stop_d, n_loop, energy_e, energy_d
global i_e_max, i_e_avg, i_e_min, i_d_max, i_d_avg, i_d_min, n_calibrated, t_calibrations
global n_lcutoff_points, l_cut_off, n_hcutoff_points, h_cut_off, calibrated
global values 

# Variable names to write to xlsx
variables = ['n_loop', 'Average encryption time', 'Average decryption time', 'Average encryption energy', 'Average decryption energy',
             'Maximum encryption current', 'Average encryption current', 'Minimum encryption current', 
             'Maximum decryption current', 'Average decryption current', 'Minimum decryption current',
             'Calibrations', 'Calibration time(s)']

# values = [n_loop, t_avg_e, t_avg_d,  energy_e, energy_d,
#           i_e_max, i_e_avg, i_e_min, i_d_max, i_d_avg, i_d_min, n_calibrated,', '.join(map(str, t_calibrations))]

data_dir = "../Data/Power/python_plots/02_csv_dir/grain_enc_dec_10x/"
output = "../Data/Power/python_plots/00_Output/grain_enc_dec_10x.txt"

data_obtained = False# True# False# True# False# True# False# True# False
energy_calc = False# True# False# True# False# True# False# True# False

# Start/end times dict
timestamps = {
    # 'ascon128_enc_dec_1000x': {'start_e': 0.88064, 'stop_e':86.84772 , 'start_d': 89.84808, 'stop_d':174.49783},
    # 'ascon128a_enc_dec_1000x': {'start_e': 0.52947, 'stop_e':60.11307, 'start_d': 63.11367 , 'stop_d':122.25007},
    # 'giftc_enc_dec_200x': {'start_e': 0.85437, 'stop_e': 156.25025, 'start_d': 159.25074, 'stop_d': 314.85193},
    # 'isapa128_enc_dec_500x': {'start_e': 0.94348, 'stop_e': 141.19371, 'start_d': 144.19465, 'stop_d': 284.03599},
    # 'isapa128a_enc_dec_500x': {'start_e': 0.99050, 'stop_e': 116.92109, 'start_d':  119.92170, 'stop_d': 235.52180},
    # 'sparkle128_enc_dec_1000x': {'start_e': 1.22652, 'stop_e': 80.71972, 'start_d': 83.72060, 'stop_d': 163.99806},
    # 'sparkle256_enc_dec_1000x': {'start_e': 1.04642, 'stop_e': 115.44641, 'start_d': 118.44648, 'stop_d': 223.41102},
    # 'tinyjambu_enc_dec_1000x': {'start_e': 1.06393, 'stop_e': 136.55366, 'start_d': 139.55458, 'stop_d': 274.98104},
    # 'xoodyak_enc_dec_1000x': {'start_e': 0.76751, 'stop_e': 217.19979, 'start_d': 220.20037, 'stop_d': 437.68150},
    # 'eleph_enc_dec_10x': {'start_e': 0.97812, 'stop_e': 120.75210, 'start_d': 123.75230, 'stop_d': 240.38971},
    'grain_enc_dec_10x': {'start_e': 1.06599, 'stop_e': 108.55892, 'start_d': 110.37643, 'stop_d': 211.14548},
    # 'photon_enc_dec_15x': {'start_e': 1.43538, 'stop_e': 176.73859, 'start_d': 179.73937, 'stop_d': 355.95881},
    # 'romulusn_enc_dec_50x': {'start_e': 1.32967, 'stop_e': 138.96308, 'start_d': 141.96324, 'stop_d': 279.46629},

    ### 'Random' second sets
    # 'photon2_enc_dec_15x': {'start_e': , 'stop_e': , 'start_d': , 'stop_d': },
    # 'ascon128a_ngnd_enc_dec_1000x': {'start_e': , 'stop_e': , 'start_d': , 'stop_d': },
    }


def main():
    global x_start_e, x_stop_e, x_start_d, x_stop_d, n_loop, energy_e, energy_d
    global i_e_max, i_e_avg, i_e_min, i_d_max, i_d_avg, i_d_min, n_calibrated, t_calibrations
    global n_lcutoff_points, l_cut_off, n_hcutoff_points, h_cut_off, calibrated
    global values
    # List all files in the directory
    files = os.listdir(data_dir)

    # Filter out only CSV files
    csv_files = [file for file in files if file.endswith('.csv')]
    csv_files = natsorted(csv_files)
    # print(csv_files)

    # Loop through each CSV file and read its contents
    dfs = []
    for file_name in csv_files:
        file_path = os.path.join(data_dir, file_name)
        # Read the CSV file into a DataFrame
        df = pd.read_csv(file_path, delimiter=';', names=["x", "y"])
        dfs.append(df)

    # Concatenate all DataFrames into a single DataFrame ignore index HAS to be true
    df = pd.concat(dfs, ignore_index=True)
    # Assuming your CSV file has columns named 'x' and 'y', change them accordingly if they are different
    x_values = df['x']/1e3
    y_values = df['y']/1e6
    # print(x_values)

    # Check for calibrations
    n_calibrated = len(y_values[y_values>1])
    t_calibrations = y_values[y_values>1].index
    calibrated = bool(n_calibrated)
    print("Number of calibrations: ", n_calibrated)

    # remove calibration current samples
    l_cut_off = []
    h_cut_off = 1
    n_hcutoff_points =  len(y_values[y_values>h_cut_off])
    # print("Calibrated: ", calibrated)
    if len(y_values[y_values<0.005]) < 10:
        l_cut_off = 0.005
        n_lcutoff_points =  len(y_values[y_values<l_cut_off])
        x_values = x_values[y_values<1]
        x_values = x_values[y_values>0.005]
        y_values = y_values[y_values<1]
        y_values = y_values[y_values>0.005]
        print("Removed " + str(n_lcutoff_points) + " at 0.005 A cut-off")
        print("Removed " + str(n_hcutoff_points) + " above " + str(h_cut_off) + "A")
    else:
        l_cut_off = 0.001
        n_lcutoff_points =  len(y_values[y_values<l_cut_off])
        x_values = x_values[y_values<1]
        x_values = x_values[y_values>0.001]
        y_values = y_values[y_values<1]
        y_values = y_values[y_values>0.001]
        print("Removed " + str(n_lcutoff_points) + " below " + str(l_cut_off) + "A")
        print("Removed " + str(n_hcutoff_points) + " above " + str(h_cut_off) + "A")

    ## get start/end time - for report
    # specific_x_val = 3.99519
    # index_of_x = x_values[x_values== specific_x_val].index[0]
    # x_values = x_values[index_of_x:]
    # y_values = y_values[index_of_x:]
    # annotate_x = x_values[index_of_x]
    # annotate_y = round(y_values[index_of_x], 4)
    # # Annotate the point with its coordinates
    # plt.annotate(f'({annotate_x}, {annotate_y})', xy=(annotate_x, annotate_y), xytext=(annotate_x-2e-5, annotate_y+1e-4))
    # plt.xlim(x_values[index_of_x]-9e-5,x_values[index_of_x]+5e-5 )
    # plt.gca().yaxis.set_ticks_position('right')
    # plt.gca().yaxis.set_label_position('right')

    # Figure out n_loop
    if "1000" in data_dir:
        n_loop = 1000
    elif "500" in data_dir:
        n_loop = 500
    elif "200" in data_dir:
        n_loop = 200
    elif "100" in data_dir:
        n_loop = 100
    elif "50" in data_dir:
        n_loop = 50
    elif "15" in data_dir:
        n_loop = 15
    elif "10" in data_dir:
        n_loop = 10
    else: 
        print("N_loop not recognised")
        os._exit

    if energy_calc:
        ## try nested dict
        x_start_e = 0
        x_stop_e = 0
        x_start_d = 0
        x_stop_d = 0

        for app, data in timestamps.items():
            if app in data_dir:
                x_start_e = data['start_e']
                x_stop_e = data['stop_e']
                x_start_d = data['start_d']
                x_stop_d = data['stop_d']

        # energy calculation - encryption
        # x_start_e = 1.180800
        # x_stop_e = 182.506460
        id_x_start_e = x_values[round(x_values,5)== x_start_e].index[0]
        # print("Start enc time: ", x_values[id_x_start_e])
        id_x_stop_e = x_values[round(x_values,5)== x_stop_e].index[0]
        i_e_avg = round(y_values[id_x_start_e:id_x_stop_e].mean(),5)
        i_e_max = round(y_values[id_x_start_e:id_x_stop_e].max(),5)
        i_e_min = round(y_values[id_x_start_e:id_x_stop_e].min(),5)
        energy_e = i_e_avg * 3.3 * (x_stop_e-x_start_e)/n_loop
        energy_e = round(energy_e,5)
        print("Average energy - encryption: ", energy_e, (x_stop_e-x_start_e)/n_loop)

        # energy calculation - decryption
        # x_start_d = 185.512820
        # x_stop_d = 367.872190
        id_x_start_d = x_values[round(x_values,5)== x_start_d].index[0]
        id_x_stop_d = x_values[round(x_values,5)== x_stop_d].index[0]
        # print("Start dec time: ", x_values[id_x_start_d])
        i_d_avg = round(y_values[id_x_start_d:id_x_stop_d].mean(),5)
        i_d_max = round(y_values[id_x_start_d:id_x_stop_d].max(),5)
        i_d_min = round(y_values[id_x_start_d:id_x_stop_d].min(),5)
        energy_d = i_d_avg * 3.3 * (x_stop_d-x_start_d)/n_loop
        energy_d = round(energy_d,5)
        print("Average energy - decryption: ", energy_d, x_start_d, id_x_start_d, x_stop_d, id_x_stop_d, (x_stop_d-x_start_d)/n_loop)

        # # store relevant values
        t_avg_e = round((x_stop_e-x_start_e)/n_loop,5)
        t_avg_d = round((x_stop_d-x_start_d)/n_loop,5)

        # Store results in a text file
        f = open(output, "w") 
        f.close()
        f = open(output, "a")
        if f.closed:
            print("File not open!!")
        f.write(output)
        f.write(": \n")
        f.write("Execution time: \n")
        f.write("\tEncryption start time:\t\t%f s\n" % x_start_e)
        f.write("\tEncryption end time:\t\t%f s\n" % x_stop_e)
        f.write("\tDecryption start time:\t\t%f s\n" % x_start_d)
        f.write("\tDecryption end time:\t\t%f s\n" % x_stop_d)
        f.write("\tAverage encryption time:\t%f s\n" % (t_avg_e))
        f.write("\tAverage decryption time:\t%f s\n\n" % (t_avg_d))
        f.write("Energy consumption: \n")
        f.write("\tAverage encryption energy: \t%f J\n" % energy_e)
        f.write("\tAverage decryption energy: \t%f J\n\n" % energy_d)
        f.write("Encryption current: \n")
        f.write("\tMaximum encryption current: %f A\n" % i_e_max)
        f.write("\tAverage encryption current: %f A\n" % i_e_avg)
        f.write("\tMinimum encryption current: %f A\n\n" % i_e_min)
        f.write("Decryption current: \n")
        f.write("\tMaximum decryption current: %f A\n" % i_d_max)
        f.write("\tAverage decryption current: %f A\n" % i_d_avg)
        f.write("\tMinimum decryption current: %f A\n\n" % i_d_min)
        f.write("Notes: \n")
        f.write("\tN_LOOP:\t\t\t\t\t\t%d\n" % n_loop)
        # f.write("\tCalibrated: \t\t\t\t%s\n" % calibrated)
        f.write("\tNum Calibrations: \t\t\t%d\n" % n_calibrated)
        f.write("\tCalibration time(s): \t\t")
        if not calibrated:
            f.write("N/A")
        else:
            for t in t_calibrations:
                f.write("%.5f s\t" % (round(t/100000,5)))
            f.write("\n\tRemoved " + str(n_lcutoff_points) + " samples(s) below " + str(l_cut_off) + "A")
            f.write(" and " + str(n_hcutoff_points) + " above " + str(h_cut_off) + "A")
        f.close
        values = [n_loop, t_avg_e, t_avg_d,  energy_e, energy_d,
          i_e_max, i_e_avg, i_e_min, i_d_max, i_d_avg, i_d_min, n_calibrated,', '.join(map(str, t_calibrations/100000))]


    else: 
        # Plotting the graph
        plt.plot(x_values, y_values, linestyle='-')  # You can customize the marker and linestyle as needed
        plt.xlabel('Time (s)')  # Replace 'X Axis Label' with your desired label
        plt.ylabel('Current (A)')  # Replace 'Y Axis Label' with your desired label
        plt.title('Example of program start and end')  # Replace 'Title of the Graph' with your desired title


        # plt.text(4.28073, 0.047, "Start of program", fontsize=12, ha='center', va='center', color='black')
        plt.grid(True)  # Add gridlines if needed
        plt.show()


def writexl(col, row):
    global values

    # open workbook
    workbook = xl.load_workbook("../Data/Data_m7.xlsx")
    ws = workbook['Power - M7']

    # Write the variable names to the first row
    # for row, var in enumerate(variables, start=row):
    #     ws.cell(row=row, column=col-1, value=var)
    # row = row_start
    for row, var in enumerate(values, start=row):
        ws.cell(row=row, column=col, value=var)
    workbook.save("../Data/Data_m7.xlsx")

if data_obtained:
    col = 3
    row = 3
    for app, data in timestamps.items():
        data_dir = "../Data/Power/python_plots/02_csv_dir/" + app + "/"
        output = "../Data/Power/python_plots/00_Output/" + app + ".txt"
        print(app)
        main()
        writexl(col, row)
        col +=1
else: 
    main()
