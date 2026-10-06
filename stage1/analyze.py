import glob
import math
import sys

import pandas as pd

MU_RHO={0.5:0.09687,1.0:0.07072,2.0:0.04942,5.0:0.03031,10.0:0.02219}
RHO=1.0 #g/cm*3
THICKNESS=10.0 #cm


def main():
    directory=sys.argv[1] if len(sys.argv)>1 else "."
    energy=float(sys.argv[2]) if len(sys.argv)>2 else 1.0

    files = sorted(glob.glob(f"{directory}/water_box_nt_result*.csv"))

    if not files:
        sys.exit(f"no CSV in {directory}")
    df=pd.concat(
        pd.read_csv(f,comment="#",header=None,names=["interacted","process","z_mm"])
        for f in files
    )


    n=len(df)
    p=1.0-df["interacted"].mean()
    err=math.sqrt(p*(1.0-p)/n)
    expected=math.exp(-MU_RHO[energy]*RHO*THICKNESS)

    print(f"E= {energy} Mev, N={n}")

    print(f"pass-through: {p:.4f} +- {err:.4f}   (XCOM: {expected:.4f}, "
              f"diff = {(p / expected - 1) * 100:+.2f} %, {(p - expected) / err:+.1f} sigma)")
    print()
    print("first process:")
    print(df["process"].value_counts().to_string())


if __name__ == "__main__":
    main()
