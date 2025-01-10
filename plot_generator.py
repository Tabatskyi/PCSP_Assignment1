import os
import pandas as pd
import matplotlib.pyplot as plt

directory = os.getcwd()

for filename in os.listdir(directory):
    if filename.endswith('.csv'):
        data = pd.read_csv(os.path.join(directory, filename))

        x_column = data.columns[0] 
        y_column = data.columns[1] 
        fig, ax = plt.subplots() 
        ax.plot(data[x_column], data[y_column])

        ax.invert_yaxis()

        plot_filename = os.path.splitext(filename)[0] + '.png'
        plt.savefig(os.path.join(directory, plot_filename))
        plt.close(fig)
