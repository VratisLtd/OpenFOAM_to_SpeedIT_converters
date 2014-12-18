#ifndef __OF2SPEEDIT_DEFINES_H__
#define __OF2SPEEDIT_DEFINES_H__

////////////////////////////////////////////////////////////////////////////////

#define DATA_DIR_NAME      "./sitf"
#define FILE_NAME_MESH     "mesh"
#define FILE_NAME_SETTINGS "settings"

////////////////////////////////////////////////////////////////////////////////

#define FILE_HEADER         "SITF_FILE_1.0"

////////////////////////////////////////////////////////////////////////////////

#define VECTOR_HEADER       "SITF_VECTOR"

#define DATA_DOUBLE         "SITF_DOUBLE"
#define DATA_INT            "SITF_INT"

////////////////////////////////////////////////////////////////////////////////

#define FIELD_HEADER              "SITF_FIELD"

#define CELL_VALS_HEADER          "SITF_CELL_VALUES_HEADER"
#define BFACE_VALS_HEADER         "SITF_BOUNDARY_FACE_VALUES_HEADER"
#define BFACE_TYPES_HEADER        "SITF_BOUNDARY_FACE_TYPES_HEADER"
#define BFACE_REFVALS_HEADER      "SITF_BOUNDARY_FACE_REFERENCE_VALUES_HEADER"
#define FIELD_OF_HEADER           "SITF_OF_FIELD_HEADER"
#define FIELD_OF_FOOTER           "SITF_OF_FIELD_FOOTER"
#define FIELD_OF_BOUNDARY_PATCHES "SITF_OF_BOUNDARY_PATCHES"

////////////////////////////////////////////////////////////////////////////////

#define MESH_HEADER                 "SITF_MESH"

#define IFACE_AREA_VECTORS_HEADER   "SITF_IFACE_AREA_VECTORS"
#define IFACE_WEIGHTS_HEADER        "SITF_IFACE_WEIGHTS"
#define IFACE_DELTA_COEFFS_HEADER   "SITF_IFACE_DELTA_COEFFS"
#define IFACE_CORRECTION_VEC_HEADER "SITF_IFACE_CORRECTION_VECTORS"
#define IFACE_OWNERS_HEADER         "SITF_IFACE_OWNERS"
#define IFACE_NEIGHBOURS_HEADER     "SITF_IFACE_NEIGHBOURS"

#define BFACE_AREA_VECTORS_HEADER   "SITF_BFACE_AREA_VECTORS"
#define BFACE_WEIGHTS_HEADER        "SITF_BFACE_WEIGHTS"
#define BFACE_DELTA_COEFFS_HEADER   "SITF_BFACE_DELTA_COEFFS"
#define BFACE_CORRECTION_VEC_HEADER "SITF_BFACE_CORRECTION_VECTORS"
#define BFACE_OWNERS_HEADER         "SITF_BFACE_OWNERS"
#define BFACE_REORDER_TABLE_HEADER  "SITF_BFACE_REORDER_TABLE"

#define CELL_VOLUMES_HEADER         "SITF_CELL_VOLUMES"

////////////////////////////////////////////////////////////////////////////////

#define SETTINGS_HEADER             "SITF_SETTINGS"

#define SETTING_SOLVER_NAME         "solver_name"
#define SOLVER_PISO                 "PISO"
#define SOLVER_SIMPLE               "SIMPLE"
#define SOLVER_PISO_TURBULENCE	    "PISOTurbulence"
#define SOLVER_SIMPLE_TURBULENCE	"SIMPLETurbulence"
#define SETTING_N_TIME_STEPS        "n_time_steps"
#define SETTINGS_DELTA_T            "delta_t"
#define SETTING_P_FIXED_CELL_IDX    "p_fixed_cell_idx"
#define SETTING_P_FIXED_CELL_VALUE  "p_fixed_cell_value"
#define SETTING_NU                  "nu"
#define SETTING_N_NONRTH_CORR_ITER  "n_nonorthogonal_correction_iterations"
#define SETTING_N_PISO_CORR_ITER    "n_PISO_correction_iterations"

#define SETTING_UEQN_MAX_ITER       "U_solver_max_iterations"
#define SETTING_UEQN_REQ_TOLERANCE  "U_solver_required_tolerance"
#define SETTING_UEQN_REL_TOLERANCE  "U_solver_relative_tolerance"
#define SETTING_UEQN_PRECOND        "U_solver_preconditioner"

#define SETTING_PEQN_MAX_ITER       "p_solver_max_iterations"
#define SETTING_PEQN_REQ_TOLERANCE  "p_solver_required_tolerance"
#define SETTING_PEQN_REL_TOLERANCE  "p_solver_relative_tolerance"
#define SETTING_PEQN_PRECOND        "p_solver_preconditioner"

#define SETTING_KEQN_MAX_ITER       "k_solver_max_iterations"
#define SETTING_KEQN_REQ_TOLERANCE  "k_solver_required_tolerance"
#define SETTING_KEQN_REL_TOLERANCE  "k_solver_relative_tolerance"
#define SETTING_KEQN_PRECOND        "k_solver_preconditioner"

#define SETTING_OMEGAEQN_MAX_ITER       "omega_solver_max_iterations"
#define SETTING_OMEGAEQN_REQ_TOLERANCE  "omega_solver_required_tolerance"
#define SETTING_OMEGAEQN_REL_TOLERANCE  "omega_solver_relative_tolerance"
#define SETTING_OMEGAEQN_PRECOND        "omega_solver_preconditioner"

#define SETTING_U_RELAXATION_FACTOR     "U_relaxation_factor"
#define SETTING_P_RELAXATION_FACTOR     "p_relaxation_factor"
#define SETTING_K_RELAXATION_FACTOR     "k_relaxation_factor"
#define SETTING_OMEGA_RELAXATION_FACTOR "omega_relaxation_factor"

#define SETTING_KOMEGASST_ALPHAK1     "kOmegaSST_alphaK1_coefficient"
#define SETTING_KOMEGASST_ALPHAK2     "kOmegaSST_alphaK2_coefficient"
#define SETTING_KOMEGASST_ALPHAOMEGA1 "kOmegaSST_alphaOmega1_coefficient"
#define SETTING_KOMEGASST_ALPHAOMEGA2 "kOmegaSST_alphaOmega2_coefficient"
#define SETTING_KOMEGASST_GAMMA1      "kOmegaSST_gamma1_coefficient"
#define SETTING_KOMEGASST_GAMMA2      "kOmegaSST_gamma2_coefficient"
#define SETTING_KOMEGASST_BETA1       "kOmegaSST_beta1_coefficient"
#define SETTING_KOMEGASST_BETA2       "kOmegaSST_beta2_coefficient"
#define SETTING_KOMEGASST_BETASTAR    "kOmegaSST_betaStar_coefficient"
#define SETTING_KOMEGASST_A1          "kOmegaSST_a1_coefficient"
#define SETTING_KOMEGASST_C1          "kOmegaSST_c1_coefficient"
#define SETTING_KOMEGASST_CMU         "kOmegaSST_Cmu_coefficient"
#define SETTING_KOMEGASST_KAPPA       "kOmegaSST_kappa_coefficient"
#define SETTING_KOMEGASST_E           "kOmegaSST_E_coefficient"

#define SETTING_YPLUSLAM              "yPlusLam_coefficient"

#define SETTING_DIV_PHIU            "div_phiU_scheme"
#define SETTING_DIV_PHIK            "div_phik_scheme"
#define SETTING_DIV_PHIOMEGA        "div_phiomega_scheme"
#define SETTING_DIV_DEV_U           "div_dev_U_scheme"

////////////////////////////////////////////////////////////////////////////////

#define TIME_VARYING_BOUNDARY_HEADER             "SITF_TIME_VARYING_BOUNDARY"

#define TIME_VARYING_BOUNDARY_PATCH_HEADER       "SITF_TIME_VARYING_BOUNDARY_PATCH"
#define TIME_VARYING_PATCH_FACE_POSITIONS_HEADER "SITF_TIME_VARYING_PATCH_FACES"
#define TIME_VARYING_PATCH_VALUES_HEADER         "SITF_TIME_VARYING_PATCH_VALUES"

////////////////////////////////////////////////////////////////////////////////

#define GROOVY_BOUNDARY_HEADER             "SITF_GROOVY_BOUNDARY"

#define GROOVY_BOUNDARY_PATCH_HEADER       "SITF_GROOVY_BOUNDARY_PATCH"
#define GROOVY_PATCH_FACE_POSITIONS_HEADER "SITF_GROOVY_PATCH_FACES"
#define GROOVY_PATCH_VALUES_HEADER         "SITF_GROOVY_PATCH_VALUES"

////////////////////////////////////////////////////////////////////////////////

#define GROOVY_TIMELINES_HEADER             "SITF_GROOVY_TIMELINES"

#define GROOVY_TIMELINES_PATCH_HEADER       "SITF_GROOVY_PATCH_TIMELINES"
#define GROOVY_TIMELINES_TIME_HEADER       "SITF_GROOVY_TIME_TIMELINES"
#define GROOVY_TIMELINES_VALUES_HEADER       "SITF_GROOVY_VALUES_TIMELINES"

////////////////////////////////////////////////////////////////////////////////

#define TOTALPRESSURE_BOUNDARY_HEADER             "SITF_TOTALPRESSURE_BOUNDARY"

#define TOTALPRESSURE_BOUNDARY_PATCH_HEADER       "SITF_TOTALPRESSURE_BOUNDARY_PATCH"
#define TOTALPRESSURE_PATCH_FACE_POSITIONS_HEADER "SITF_TOTALPRESSURE_PATCH_FACES"
#define TOTALPRESSURE_PATCH_VALUES_HEADER         "SITF_TOTALPRESSURE_PATCH_VALUES"

////////////////////////////////////////////////////////////////////////////////

#endif
