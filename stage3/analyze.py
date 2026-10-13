import glob
import os
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

ME = 0.51099895  # 電子の静止エネルギー [MeV]
DENSITY = 6.71   # GSO の密度 [g/cm3]

EVENT_COLUMNS = ["event", "e0_MeV", "x0_mm", "y0_mm", "z0_mm", "dx0", "dy0", "dz0",
                 "n_interactions", "n_hits", "edep_MeV", "edep_step_MeV"]
INTERACTION_COLUMNS = [
    "event", "id", "parent", "track", "creator", "process", "x_mm", "y_mm", "z_mm", "t_ns",
    "e_in_MeV", "dx_in", "dy_in", "dz_in",
    "e_out_MeV", "dx_out", "dy_out", "dz_out",
    "e_e_MeV", "dx_e", "dy_e", "dz_e",
    "e_pos_MeV", "e_charged_MeV", "n_secondaries",
]
HIT_COLUMNS = ["event", "track", "origin", "particle", "x_mm", "y_mm", "z_mm", "t_ns", "edep_MeV"]


def load(directory, table, columns):
    """スレッドごとの CSV（gso_box_nt_<table>_t*.csv）をまとめて読む"""
    files = sorted(glob.glob(os.path.join(directory, f"gso_box_nt_{table}_t*.csv")))
    if not files:
        sys.exit(f"no {table} CSV in {directory}")
    return pd.concat([pd.read_csv(f, comment="#", header=None, names=columns) for f in files],
                     ignore_index=True)


def reaction_sum(interactions):
    
    df = interactions.copy()
    
    df["skip"] = df.creator.isin(["eBrem", "annihil"])
    while True:
        parent_skip = df.set_index(["event", "id"]).skip
        inherited = pd.MultiIndex.from_arrays([df.event, df.parent]).map(parent_skip.to_dict().get)
        new = df.skip | (pd.Series(inherited, index=df.index) == True)
        if new.equals(df.skip):
            break
        df["skip"] = new
    given = df.e_charged_MeV + np.where(df.process == "conv", 2 * ME, 0.)
    return given[~df.skip].groupby(df.event[~df.skip]).sum()


def fraction(mask, n):
    k = int(np.sum(mask))
    p = k / n
    return f"{p:.4f} ± {np.sqrt(p * (1 - p) / n):.4f}  ({k})"


def main():
    directory = sys.argv[1]
    e0 = float(sys.argv[2])
    thickness = float(sys.argv[3])

    events = load(directory, "event", EVENT_COLUMNS)
    interactions = load(directory, "interaction", INTERACTION_COLUMNS)
    hits = load(directory, "hit", HIT_COLUMNS)
    n = len(events)
    print(f"E = {e0} MeV, GSO thickness = {thickness} cm, {n} events")

    
    hit_sum = hits.groupby("event")["edep_MeV"].sum().reindex(events["event"], fill_value=0.).to_numpy()
    print("\n[1] self-check (should be ~1e-5 or less: CSV keeps 6 digits)")
    print(f"  max |edep - edep_step| = {np.max(np.abs(events.edep_MeV - events.edep_step_MeV)):.2e} MeV")
    print(f"  max |edep - sum(hits)| = {np.max(np.abs(events.edep_MeV - hit_sum)):.2e} MeV")

    
    interacted = events.n_interactions > 0
    p = interacted.mean()
    mu = -np.log(1 - p) / thickness
    print("\n[2] interaction probability (compare with 1 - exp(-mu t), mu from XCOM)")
    print(f"  interacted: {fraction(interacted, n)}")
    print(f"  -> mu = {mu:.4f} /cm, mu/rho = {mu / DENSITY:.5f} cm2/g")

    
    edep = events.edep_MeV.to_numpy()
    tol = 0.001 
    peaks = {
        "full absorption (E)": (e0 - tol, e0 + tol),
        "Gd K x-ray escape (E - 43..50 keV)": (e0 - 0.0510, e0 - 0.0415),
    }
    if e0 > 2 * ME:
        peaks["single escape (E - 0.511)"] = (e0 - ME - tol, e0 - ME + tol)
        peaks["double escape (E - 1.022)"] = (e0 - 2 * ME - tol, e0 - 2 * ME + tol)
    print("\n[3] peaks (fraction of all events)")
    for name, (lo, hi) in peaks.items():
        print(f"  {name:36s}: {fraction((edep > lo) & (edep < hi), n)}")
    full = np.abs(edep - e0) < tol
    print(f"  photofraction (full / interacted)  : {np.sum(full) / np.sum(interacted):.4f}")
    edge = e0 * (2 * e0 / ME) / (1 + 2 * e0 / ME)
    print(f"  Compton edge (single scatter)      : {edge:.4f} MeV")

    
    react = reaction_sum(interactions).reindex(events["event"], fill_value=0.).to_numpy()
    diff = react - edep
    print("\n[4] reaction sum (energy given to e-/e+ by gammas) vs deposited energy")
    print(f"  events with reaction sum > E (should be 0)   : {int(np.sum(react > e0 + tol))}")
    print(f"  events with reaction sum < edep (should be 0): {int(np.sum(diff < -tol))}")
    print(f"  events with reaction sum > edep (> 1 keV)    : {fraction(diff > tol, n)}")
    print(f"  full absorption by reaction sum              : {fraction(np.abs(react - e0) < tol, n)}")
    if e0 > 2 * ME:
        for name, x in [("single escape", e0 - ME), ("double escape", e0 - 2 * ME)]:
            print(f"  {name} by reaction sum               : {fraction(np.abs(react - x) < tol, n)}")
    print(f"  mean (reaction sum - edep)                   : {diff.mean() * 1000:.2f} keV")


    by_particle = hits.groupby("particle")["edep_MeV"].sum()
    print("\n[5] deposited energy by particle (share of total)")
    for name, value in by_particle.sort_values(ascending=False).items():
        print(f"  {name:6s}: {value / by_particle.sum():.4f}")
    print("  interactions by process (all gammas in GSO):")
    for name, k in interactions.process.value_counts().items():
        print(f"    {name:6s}: {k}")

    
    fig, axes = plt.subplots(1, 2, figsize=(12, 4.5))
    width = 0.002 if e0 < 2 else 0.010  
    bins = np.arange(-0.5 * width, e0 * 1.05 + width, width)  
    ax = axes[0]
    ax.hist(edep[edep > 0], bins=bins, histtype="step", color="C0", label="deposited (edep)")
    ax.hist(react[react > 0], bins=bins, histtype="step", color="C1", alpha=0.7, label="reaction sum")
    marks = {"E": e0, "Compton edge": edge}
    if e0 > 2 * ME:
        marks.update({"E-0.511": e0 - ME, "E-1.022": e0 - 2 * ME})
    for name, x in marks.items():
        ax.axvline(x, color="gray", lw=0.6, ls="--")
        ax.text(x, 0.97, name, rotation=90, va="top", ha="right", fontsize=8,
                transform=ax.get_xaxis_transform())
    ax.set_yscale("log")
    ax.set_xlabel("energy in GSO [MeV]")
    ax.set_ylabel(f"events / {width * 1000:g} keV")
    ax.set_title(f"{e0} MeV gamma, GSO {thickness} cm")
    ax.legend(loc="upper left", fontsize=8)

    ax = axes[1]  
    zoom = np.arange(e0 - 0.07025, e0 + 0.005, 0.0005)
    ax.hist(edep, bins=zoom, histtype="step", color="C0")
    ax.axvspan(e0 - 0.0510, e0 - 0.0415, color="C2", alpha=0.15, label="Gd K x-ray escape")
    ax.set_yscale("log")
    ax.set_xlabel("energy in GSO [MeV]")
    ax.set_ylabel("events / 0.5 keV")
    ax.set_title("zoom below the full-absorption peak")
    ax.legend(loc="upper left", fontsize=8)

    fig.tight_layout()
    out = os.path.join(directory, f"stage3_E{e0:g}.png")
    fig.savefig(out, dpi=120)
    print(f"\nwrote {out}")


if __name__ == "__main__":
    main()
