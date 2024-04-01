import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
data_dir = "../Data/Power/python_plots/idd_current/"
# Read the CSV file into a DataFrame
# df = pd.read_csv('Data/2024_03_29_08_57_01_32_rawfile_13.csv', delimiter=';', names=['x', 'y'])  # Replace 'data.csv' with the path to your CSV file

filenames = ['2024_03_29_14_54_33_1_rawfile_5.csv', '2024_03_29_14_54_33_1_rawfile_6.csv', '2024_03_29_14_54_33_1_rawfile_7.csv',
            '2024_03_29_14_54_33_1_rawfile_8.csv', '2024_03_29_14_54_33_1_rawfile_9.csv', '2024_03_29_14_54_33_1_rawfile_10.csv',
            '2024_03_29_14_54_33_1_rawfile_11.csv', '2024_03_29_14_54_33_1_rawfile_12.csv']
dfs_idd = []

for filename in filenames:
    df = pd.read_csv(data_dir + filename, delimiter=';', names=["x", "y"])
    dfs_idd.append(df)

# Concatenate all DataFrames into a single DataFrame
df_idd = pd.concat(dfs_idd, ignore_index=True)


# print(combined_df)
df = df_idd

# Assuming your CSV file has columns named 'x' and 'y', change them accordingly if they are different
x_values = df['x']/1e3
y_values = df['y']/1e6
# x_values = x_values[:656000]
# y_values = y_values[:656000]

# start_idx = x_3v3[x_3v3 == "4.00000"].index[0]
# print(start_idx)

# calculate energy


# Plotting the graph
plt.plot(x_values, y_values,linestyle='-')  # You can customize the marker and linestyle as needed
plt.xlabel('Time (s)')  # Replace 'X Axis Label' with your desired label
plt.ylabel('Current (A)')  # Replace 'Y Axis Label' with your desired label
plt.title('Example of program start and end')  # Replace 'Title of the Graph' with your desired title
# plt.text(4.28073, 0.047, "Start of program", fontsize=12, ha='center', va='center', color='black')
plt.grid(True)  # Add gridlines if needed
plt.show()


