```mermaid
gitGraph
   %% --- Initial commits on main (stable base) ---
   commit id:"Init" msg:"Initial commit (main) [Alice]"
   commit id:"M1" msg:"Main release (main) [Bob]"

   %% --- Create protected subsystem branches off main ---
   branch pds_branch_protected
   commit id:"PDS1" msg:"PDS stable commit [Charlie]"
   branch feature/PDS-Feature1
   commit id:"PDSF1" msg:"Implement new PDS sensor integration [Dave]"
   merge pds_branch_protected tag:"Merge PDS Feature [Eve]"
   checkout main

   branch bms_branch_protected
   commit id:"BMS1" msg:"BMS stable commit [Frank]"
   branch feature/BMS-Feature1
   commit id:"BMSF1" msg:"Improve BMS thermal monitoring [Grace]"
   merge bms_branch_protected tag:"Merge BMS Feature [Heidi]"
   checkout main

   branch cdh_branch_protected
   commit id:"CDH1" msg:"CDH stable commit [Ivan]"
   branch feature/CDH-Feature1
   commit id:"CDHF1" msg:"Refactor CDH state machine [Judy]"
   merge cdh_branch_protected tag:"Merge CDH Feature [Kevin]"
   checkout main

   branch backplane_branch_protected
   commit id:"BP1" msg:"Backplane stable commit [Laura]"
   branch feature/BP-Feature1
   commit id:"BPF1" msg:"Add diagnostics to backplane [Mallory]"
   merge backplane_branch_protected tag:"Merge Backplane Feature [Niaj]"
   checkout main

   %% --- Final integration: merge all protected subsystem branches into main ---
   merge pds_branch_protected tag:"Integrate PDS [Olivia]"
   merge bms_branch_protected tag:"Integrate BMS [Peggy]"
   merge cdh_branch_protected tag:"Integrate CDH [Quentin]"
   merge backplane_branch_protected tag:"Integrate Backplane [Rupert]"
```

How This Branching Strategy Works
**Main Branch (Stable Base):**
The main branch serves as your production-ready, stable base. All releases are tagged here (e.g. “Main release (main) [Bob]”). No direct development happens on main—only thoroughly tested code is merged into it.

**Protected Subsystem Branches:**
Each subsystem—PDS, BMS, CDH, and Backplane—has its own protected branch (pds_branch_protected, bms_branch_protected, cdh_branch_protected, backplane_branch_protected). These branches are dedicated to their respective modules and are only updated via pull requests that include compile checks and tests. This keeps the code for each subsystem stable.

**Feature Branches:**
When a developer needs to implement a new feature (e.g. new sensor integration for PDS or a refactor for CDH), they create a feature branch off the appropriate protected branch. Once the feature is complete and fully tested, the branch is merged back into its protected branch. This process helps isolate development work from the stable code.

**Integration into Main:**
Periodically, the protected branches are merged back into main. This step integrates all the subsystem changes into a single, stable release candidate. Because the protected branches only contain working code, the main branch remains reliable.

This model (inspired by the nvie branching model) helps maintain a clear separation between development and production code. It allows teams to work concurrently on different subsystems while ensuring that only quality, tested code makes it into your production release. Happy branching!
