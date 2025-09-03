import pandas as pd

# 讀取原始檔案
df = pd.read_csv(r"C:\Users\jianh\OneDrive\Desktop\ToonsRobotics\safmc\safmc_uwb\simulate_localization\0813_3tag_1anchor_test_data\serial_data_line 7_20250813_163627.csv")

# 篩選出 Anchor EUI & Est_X沒有值的 row
filtered_df = df[df['Anchor EUI'].isna() | (df['Anchor EUI'] == '')]
filtered_df = filtered_df[filtered_df['Est_X'].isna() | (filtered_df['Est_X'] == '')]
# 儲存為新檔案
filtered_df.to_csv(r'C:\Users\jianh\OneDrive\Desktop\ToonsRobotics\safmc\safmc_uwb\simulate_localization\0813_3tag_1anchor_test_data\line7.csv', index=False)