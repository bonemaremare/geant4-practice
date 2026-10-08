import glob
import math
import sys

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

ME=0.51099895 #電子の静止エネルギー(mev)

EVENT_COLUMS=["event","e0_MeV","x0_mm","y0_mm","z0_mm","dx0","dy0","dz0","n_interactions"]
INTERACTION_COLUMNS=[
    "event","id","parent","track","process","x_mm","y_mm","z_mm","t_ns","e_in_MeV","dx_in","dy_in","dz_in",
    "e_out_MeV","dx_out","dy_out","dz_out","e_e_MeV","dx_e","dy_e","dz_e", "e_pos_MeV","n_secondaries",
]

def load(directory,table,columns):
    files=sorted(glob.glob(f"{directory}/water_box_nt_{table}*.csv"))
    if not files:
        sys.exit(f"no(table) CSV in {directory}")
    return pd.concat(pd.read_csv(f,comment="#",header=None,names=columns) for f in files)

def vec(df,suffix):
    return df[[f"dx_{suffix}",f"dy_{suffix}",f"dz_{suffix}"]].to_numpy()

def klein_nishina(cos_theta,energy):
    ratio=1.0/(1.0+energy/ME*(1.0-cos_theta))
    return ratio**2 *(ratio+1.0/ratio-(1.0-cos_theta**2))

def main():
    directory=sys.argv[1] if len(sys.argv)>1 else "."
    energy=float(sys.argv[2]) if len(sys.argv)>2 else 1.0

    events=load(directory,"event",EVENT_COLUMS)
    inter=load(directory,"interaction",INTERACTION_COLUMNS)
    n_events=len(events)


    print(f"E = {energy} MeV, N = {n_events} events, {len(inter)} interactions in water")
    print(f"  events with >= 1 interaction: {(events['n_interactions'] > 0).mean():.4f}")

    assert events["n_interactions"].sum() == len(inter), "event and interaction tables disagree"

    
    first = inter[inter["id"] == 0]
    fractions = first["process"].value_counts(normalize=True)
    print("first interaction: " + ", ".join(f"{p} {f:.4f}" for p, f in fractions.items()))

    
    print("\ninteractions by who interacted:")
    who = np.where(inter["track"] == 1, "primary gamma", "other gamma")
    print(pd.crosstab(inter["process"], who, colnames=["who"]).to_string())

    
    c = first[first["process"] == "compt"]
    d_in, d_out, d_e = vec(c, "in"), vec(c, "out"), vec(c, "e")
    e_in, e_out, t_e = c["e_in_MeV"].to_numpy(), c["e_out_MeV"].to_numpy(), c["e_e_MeV"].to_numpy()
    cos_theta = np.sum(d_in * d_out, axis=1)


    e_formula = e_in / (1.0 + e_in / ME * (1.0 - cos_theta))

    p_e = np.sqrt(t_e * (t_e + 2.0 * ME))
    momentum_miss = np.linalg.norm(e_in[:, None] * d_in - e_out[:, None] * d_out - p_e[:, None] * d_e, axis=1)
    phi_gamma = np.degrees(np.arctan2(d_out[:, 1], d_out[:, 0]))
    phi_electron = np.degrees(np.arctan2(d_e[:, 1], d_e[:, 0]))
    dphi = (phi_electron - phi_gamma) % 360.0

    print(f"\nfirst Compton of the primary: {len(c)} events")
    print(f"  max |E'/formula - 1|        = {np.max(np.abs(e_out / e_formula - 1)):.2e}")
    print(f"  max |E - E' - T| / E        = {np.max(np.abs(e_in - e_out - t_e) / e_in):.2e}")
    print(f"  max |momentum miss| / E     = {np.max(momentum_miss / e_in):.2e}")
    print(f"  azimuth difference          = {dphi.mean():.3f} +- {dphi.std():.3f} deg (expect 180)")

    bins = np.linspace(-1.0, 1.0, 41)
    counts, _ = np.histogram(cos_theta, bins=bins)
    fine = np.linspace(-1.0, 1.0, 40001)
    density = klein_nishina(fine, energy)
    cumulative = np.concatenate([[0.0], np.cumsum((density[1:] + density[:-1]) / 2 * np.diff(fine))])
    expected = len(c) * np.diff(np.interp(bins, fine, cumulative)) / cumulative[-1]
    pull = (counts - expected) / np.sqrt(expected)
    chi2 = np.sum(pull**2)
    print(f"  Klein-Nishina: chi2 / ndf = {chi2:.1f} / {len(counts) - 1}")

    centers = (bins[:-1] + bins[1:]) / 2
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 4))
    ax1.errorbar(centers, counts, yerr=np.sqrt(counts), fmt="o", ms=3, color="#0072B2", label="Geant4")
    ax1.step(bins, np.append(expected, expected[-1]), where="post", color="#999999", label="Klein-Nishina")
    ax1.set_xlabel("cos(theta) of scattered gamma")
    ax1.set_ylabel("events / bin")
    ax1.set_title(f"first Compton of primary, E = {energy:g} MeV")
    ax1.legend()

    ax2.hist(dphi, bins=np.linspace(0, 360, 73), color="#0072B2")
    ax2.set_xlabel("azimuth(electron) - azimuth(gamma) [deg]")
    ax2.set_ylabel("events / 5 deg")
    ax2.set_xticks([0, 90, 180, 270, 360])

    fig.tight_layout()
    out = f"{directory}/stage2_E{energy:g}.png"
    fig.savefig(out, dpi=150)
    print(f"\nfigure: {out}")


if __name__ == "__main__":
    main()


