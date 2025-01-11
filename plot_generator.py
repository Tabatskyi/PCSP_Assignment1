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
        ax.set_xlabel(x_column.capitalize())
        ax.set_ylabel(y_column.capitalize())

        ax.set_xticks(range(len(data[x_column])))
        ax.set_xticklabels(data[x_column].astype('str'))

        for i, txt in enumerate(data[y_column]):
            ax.annotate(txt, (i, data[y_column][i]))

        ax.plot(range(len(data[x_column])), data[y_column])
        ax.invert_yaxis()

        plot_filename = os.path.splitext(filename)[0] + '.png'
        plt.savefig(os.path.join(directory, plot_filename))
        plt.close(fig)
