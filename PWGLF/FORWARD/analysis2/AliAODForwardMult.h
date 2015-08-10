//
// See implementation or Doxygen comments for more information
//
#ifndef ALIAODFORWARDMULT_H
#define ALIAODFORWARDMULT_H
/**
 * @file   AliAODForwardMult.h
 * @author Christian Holm Christensen <cholm@dalsgaard.hehi.nbi.dk>
 * @date   Wed Mar 23 13:58:00 2011
 * 
 * @brief  Per-event @f$ N_{ch}@f$ per @f$(\eta,\varphi)@f$ bin 
 * 
 * @ingroup pwglf_forward_aod
 * 
 */
#include <TObject.h>
#include <TH2D.h>
class TBrowser;
class TH1I;
/**
 * Class that contains the forward multiplicity data per event 
 *
 * This class contains a histogram of 
 * @f[
 *   \frac{d^2N_{ch}}{d\eta d\phi}\quad,
 * @f]
 * as well as a trigger mask for each analysed event.  
 * 
 * The eta acceptance of the event is stored in the underflow bins of
 * the histogram.  So to build the final histogram, one needs to
 * correct for this acceptance (properly weighted by the events), and
 * the vertex efficiency.  This simply boils down to defining a 2D
 * histogram and summing the event histograms in that histogram.  One
 * should of course also do proper book-keeping of the accepted event.
 *
 * @code 
 TTree* GetAODTree()
 { 
    TFile* file = TFile::Open("AliAODs.root","READ");
    TTree* tree = static_cast<TTree*>(file->Get("aodTree"));
    return tree;
 }
 
 void Analyse()
 { 
   TH2D*              sum        = 0;                  // Summed hist
   TTree*             tree       = GetAODTree();       // AOD tree
   AliAODForwardMult* mult       = 0;                  // AOD object
   Int_t              nTriggered = 0;                  // # of triggered ev.
   Int_t              nWithVertex= 0;                  // # of ev. w/vertex
   Int_t              nAccepted  = 0;                  // # of ev. used
   Int_t              nAvailable = tree->GetEntries(); // How many entries
   Float_t            vzLow      = -10;                // Lower ip cut
   Float_t            vzHigh     =  10;                // Upper ip cut
   Int_t              mask       = AliAODForwardMult::kInel;// Trigger mask
   tree->SetBranchAddress("forward", &forward);        // Set the address
 
   for (int i = 0; i < nAvailable; i++) { 
     // Create sum histogram on first event - to match binning to input
     if (!sum) sum = static_cast<TH2D*>(mult->Clone("d2ndetadphi"));
 
     tree->GetEntry(i);
 
     // Other trigger/event requirements could be defined 
     if (!mult->IsTriggerBits(mask)) continue; 
     nTriggered++;
 
     // Check if we have vertex 
     if (!mult->HasIpZ()) continue;
     nWithVertex++;
 
     // Select vertex range (in centimeters) 
     if (!mult->InRange(vzLow, vzHigh) continue; 
     nAccepted++;
 
     // Add contribution from this event
     sum->Add(&(mult->GetHistogram()));
   }
 
   // Get acceptance normalisation from underflow bins 
   TH1D* norm   = sum->ProjectionX("norm", 0, 1, "");
   // Project onto eta axis - _ignoring_underflow_bins_!
   TH1D* dndeta = sum->Projection("dndeta", 1, -1, "e");
   // Normalize to the acceptance 
   dndeta->Divide(norm);
   // Scale by the vertex efficiency 
   dndeta->Scale(Double_t(nWithVertex)/nTriggered, "width");
   // And draw the result
   dndeta->Draw();
 }
 @endcode   
 *     
 * The above code will draw the final @f$ dN_{ch}/d\eta@f$ for the
 * selected event class and vertex range
 *
 * The histogram can be used as input for other kinds of analysis too, 
 * like flow, event-plane, centrality, and so on. 
 *
 * @ingroup pwglf_forward_aod
 */
class AliAODForwardMult : public TObject
{
public:
  /** 
   * Bits of the trigger pattern
   *
   * @ingroup pwglf_forward_aod
   */
  enum ETriggerBits { 
    /** In-elastic collision - really MBOR*/
    kInel        = 0x0001, 
    /** In-elastic collision with at least one SPD tracklet */
    kInelGt0     = 0x0002, 
    /** Non-single diffractive collision - (V0AND||FASTOR>5) */
    kNSD         = 0x0004, 
    /** Empty bunch crossing */
    kEmpty       = 0x0008, 
    /** A-side trigger */
    kA           = 0x0010, 
    /** B(arrel) trigger */
    kB           = 0x0020, 
    /** C-side trigger */
    kC           = 0x0080,  
    /** Empty trigger */
    kE           = 0x0100,
    /** pileup from SPD */
    kPileUp      = 0x0200,    
    /** true NSD from MC */
    kMCNSD       = 0x0400,    
    /** Offline MB triggered */
    kOffline     = 0x0800,
    /** At least one SPD cluster */ 
    kNClusterGt0 = 0x1000,
    /** V0-AND trigger */
    kV0AND       = 0x2000, 
    /** Satellite event */
    kSatellite   = 0x4000,
    /** SPD outlier event */
    kSPDOutlier  = 0x8000,
    /** SPD pile-up */
    kPileupSPD   = 0x10000,
    /** Track pile-up */
    kPileupTrack = 0x20000,
    /** Out of bunch pileup */
    kPileupBC    = 0x40000,
    /** SPD pile-up in mult bins */
    kPileupBins  = 0x80000,
    /** AD-OR */
    kADOR        = 0x100000,
    /** AD-AND */
    kADAND       = 0x200000,
    /** Flag that says the pattern is an OR rather than an AND */
    kInclusive   = 0x8000000,
    /** Our default negative filter */
    kDefaultFilter = kPileupBins|kSPDOutlier
  };
  /** 
   * Bin numbers in trigger histograms 
   *
   * @ingroup pwglf_forward_aod
   */
  enum ETriggerBins { 
    kBinAll=1,
    kBinInel, 
    kBinInelGt0, 
    kBinNSD, 
    kBinV0AND,
    kBinADOR,
    kBinADAND,
    kBinA, 
    kBinB, 
    kBinC, 
    kBinE,
    kBinSatellite,
    kBinMCNSD,
    kBinPileUp, 
    kBinOffline,
    kBinNClusterGt0,
    kWithTrigger, 
    kWithVertex, 
    kAccepted
  };
  /** 
   * User bits of these objects (bits 14-23 can be used)
   *
   * @ingroup pwglf_forward_aod
   */
  enum EFlags {
    /** Secondary correction maps where applied */
    kSecondary           = (1 << 14), 
    /** Vertex bias correction was applied */
    kVertexBias          = (1 << 15),  
    /** Acceptance correction was applied */
    kAcceptance          = (1 << 16), 
    /** Merging efficiency correction was applied */
    kMergingEfficiency   = (1 << 17),
    /** Signal in overlaps is the sum */
    kSum                 = (1 << 18), 
    /** Used eta dependent empirical correction - to be implemented */
    kEmpirical           = (1 << 19)
  };
  /**
   * Return codes of CheckEvent 
   *
   * @ingroup pwglf_forward_aod
   */
  enum ECheckStatus {
    /** Event accepted by cuts */
    kGoodEvent = 0, 
    /** Event centrality not in range */
    kWrongCentrality, 
    /** Event trigger isn't in the supplied mask */
    kWrongTrigger, 
    /** Event is a pile-up event */
    kIsPileup,
    /** Other filter cut */
    kIsFilterOut,
    /** Event has no interaction point information */
    kNoVertex, 
    /** Event interaction point is out of range */
    kWrongVertex,
    /** Outlier */
    kOutlierEvent
  };
    
  /** 
   * Default constructor 
   * 
   * Used by ROOT I/O sub-system - do not use
   */
  AliAODForwardMult();
  /** 
   * Constructor 
   * 
   * @param isMC Whether this was from MC or not 
   */
  AliAODForwardMult(Bool_t isMC);
  /** 
   * Destructor 
   */
  virtual ~AliAODForwardMult() {} // Destructor 
  /** 
   * Initialize 
   * 
   * @param etaAxis  Pseudo-rapidity axis
   */
  void Init(const TAxis& etaAxis);
  /** 
   * @{ 
   * @name Get the Nch 
   */
  /** 
   * Get the @f$ d^2N_{ch}/d\eta d\phi@f$ histogram, 
   *
   * @return @f$ d^2N_{ch}/d\eta d\phi@f$ histogram, 
   */  
  const TH2D& GetHistogram() const { return fHist; } // Get histogram 
  /** 
   * Get the @f$ d^2N_{ch}/d\eta d\phi@f$ histogram, 
   *
   * @return @f$ d^2N_{ch}/d\eta d\phi@f$ histogram, 
   */  
  TH2D& GetHistogram() { return fHist; } // Get histogram
  /** 
   * Get which bin the @f$\eta@f$ coverage is stored in 
   * 
   * @return The bin (in phi) that stores the coverage 
   */
  Int_t GetEtaCoverageBin() const { return 0; }
  /** 
   * Get which bin the @f$\varphi@f$ acceptance is stored in 
   * 
   * @return The bin (in phi) that stores the acceptance 
   */
  Int_t GetPhiAcceptanceBin() const { return fHist.GetNbinsY()+1; }
  /** 
   * Get the @f$\eta@f$ coverage. User must manage return 
   * 
   * @return The @f$\eta@f$ coverage. 
   */
  TH1* GetEtaCoverage() const;
  /** 
   * Get the @f$\varphi@f$ acceptance. User must manage return 
   * 
   * @return The @f$\varphi@f$ acceptance. 
   */
  TH1* GetPhiAcceptance() const;
  /** 
   * Get the @f$\eta@f$ coverage.  Note, the histogram is not resst by
   * this function, thus allowing to accumulate in the passed
   * histogram.
   * 
   * @param h On return, the @f$\eta@f$ coverage. 
   */
  void FillEtaCoverage(TH1& h) const;
  /** 
   * Get the @f$\varphi@f$ acceptance. Note, the histogram is not
   * resst by this function, thus allowing to accumulate in the passed
   * histogram.
   * 
   * @param h On return, the @f$\varphi@f$ acceptance. 
   */
  void FillPhiAcceptance(TH1& h) const;
  /* @} */
  /** 
   * @{ 
   * @name Prinary interaction point 
   */
  /** 
   * Set the z coordinate of the interaction point
   * 
   * @param ipZ Interaction point z coordinate
   */
  void SetIpZ(Float_t ipZ) { fIpZ = ipZ; } // Set Ip's Z coordinate
  /** 
   * Set the z coordinate of the interaction point
   * 
   * @return Interaction point z coordinate
   */
  Float_t GetIpZ() const { return fIpZ; } // Get Ip's Z coordinate 
  /** 
   * Check if we have a valid z coordinate of the interaction point
   *
   * @return True if we have a valid interaction point z coordinate
   */
  Bool_t HasIpZ() const;
  /** 
   * Check if the z coordinate of the interaction point is within the
   * given limits.  Note that the convention used corresponds to the
   * convention used in ROOTs TAxis.
   * 
   * @param low  Lower cut (inclusive)
   * @param high Upper cut (exclusive)
   * 
   * @return true if @f$ low \ge ipz < high@f$ 
   */
  Bool_t InRange(Float_t low, Float_t high) const;
  /* @} */
  /** 
   * @{ 
   * @name Collision system
   */
  /** 
   * Set the center of mass energy per nucleon-pair.  This is stored 
   * in the (0,0) of the histogram 
   * 
   * @param sNN Center of mass energy per nucleon pair (GeV)
   */
  void SetSNN(UShort_t sNN); 
  /** 
   * Get the collision system number
   * - 0: Unknown 
   * - 1: pp
   * - 2: PbPb
   * 
   * @param sys Collision system number
   */
  void SetSystem(UShort_t sys);
  /** 
   * Get the center of mass energy per nucleon pair (GeV)
   * 
   * @return Center of mass energy per nucleon pair (GeV)
   */
  UShort_t GetSNN() const;
  /** 
   * Get the collision system number
   * - 0: Unknown 
   * - 1: pp
   * - 2: PbPb
   * 
   * @return Collision system number
   */
  UShort_t GetSystem() const;
  /** 
   * @{ 
   * @name Centrality 
   */
  /** 
   * Set the event centrality 
   * 
   * @param c Centrality 
   */
  void SetCentrality(Float_t c) { fCentrality = c; }
  /** 
   * Get the event centrality 
   * 
   * @return Centrality 
   */
  Float_t GetCentrality() const { return fCentrality; }
  /** 
   * Check if we have a valid centrality 
   * 
   * @return true if the centrality information is valid 
   */
  Bool_t  HasCentrality() const { return !(fCentrality  < 0); }
  /* @} */
  /** 
   * @{ 
   * @name SPD clusters 
   */   
  /** 
   * Get the number of SPD clusters seen in @f$ |\eta|<1@f$ 
   * 
   * @return Number of SPD clusters seen
   */
  UShort_t GetNClusters() const { return fNClusters; }
  /** 
   * Set the number of SPD clusters seen in @f$ |\eta|<1@f$ 
   * 
   * @param n Number of SPD clusters 
   */
  void SetNClusters(UShort_t n) { fNClusters = n; }
  /* @} */
  /** 
   * @{ 
   * @name Trigger information 
   */
  /** 
   * Get the trigger bits 
   * 
   * @return Trigger bits 
   */
  UInt_t GetTriggerBits() const { return fTriggers; } // Get triggers
  /** 
   * Set the trigger mask 
   * 
   * @param trg Trigger mask
   */
  void SetTriggerMask(UInt_t trg) { fTriggers = trg; } // Set triggers 
  /** 
   * Set bit(s) in trigger mask 
   * 
   * @param bits bit(s) to set 
   */
  void SetTriggerBits(UInt_t bits) { fTriggers |= bits; } // Set trigger bits
  /** 
   * Check if all/some bit(s) are set in the trigger mask @a trg.
   * Note, this is an @e and between the bits, unless the bit
   * kInclusive is set in the @a bits argument.  If you need an @e or
   * you should use the member function IsTriggerOrBits, or set the
   * bit @c kInclusive
   * 
   * @param bits Bits to test for 
   * @param trg  Event trigger bits 
   * 
   * @return true if all/some enabled bits in the argument is also set in
   * the trigger word @a trg
   */
  static Bool_t IsTriggerBits(UInt_t bits, UInt_t trg);
  /** 
   * Check if all/some bit(s) are set in the trigger mask.  Calls the
   * static version if this member function with the second argument
   * set to fTriggers
   * 
   * @param bits Bits to test for 
   * 
   * @return true if all/some enabled bits in the argument is also set
   * in the trigger word stored in this object
   */
  Bool_t IsTriggerBits(UInt_t bits) const;
  /** 
   * Check if any of bit(s) are enabled in the trigger word.  This is
   * an @e or between the selected bits.  If you need and @a and you
   * should use the member function IsTriggerBits;
   * 
   * @param bits Bits to check for 
   * 
   * @return true if any of the enabled bits in the arguments are also
   * enabled in the trigger mask
   */
  Bool_t IsTriggerOrBits(UInt_t bits) const;
  /** 
   * Whether we have any trigger bits 
   *
   * @return true if we have some trigger 
   */
  Bool_t HasTrigger() const { return fTriggers != 0; } // Check for triggers
  /** 
   * Get a string correspondig to the trigger mask
   * 
   * @param mask Trigger mask 
   * @param sep  Field separator
   * 
   * @return Static string (copy before use)
   */
  static const Char_t* GetTriggerString(UInt_t mask, const char* sep="&");
  /** 
   * Utility function to make a trigger mask from the passed string. 
   * 
   * The string is a comma or space seperated list of case-insensitive
   * strings
   * 
   * - INEL 
   * - INEL>0
   * - NSD 
   * 
   * @param what Which triggers to put in the mask. 
   * @param sep  Field separator
   * 
   * @return The generated trigger mask. 
   */
  static UInt_t MakeTriggerMask(const char* what, const char* sep="&");
  /* @} */
  /** 
   * @{ 
   * @name Check specific trigger bits 
   */
  /**
   * Check for In-elastic collision - really MBOR*
   * 
   * @return true if kInel bit is set 
   */
  Bool_t IsInel() const { return (fTriggers & kInel); } 
  /**
   * Check for In-elastic collision with at least one SPD tracklet *
   * 
   * @return true if kInelGt0 bit is set 
   */
  Bool_t IsInelGt0() const { return (fTriggers & kInelGt0); } 
  /**
   * Check for Non-single diffractive collision - (V0AND||FASTOR>5) *
   * 
   * @return true if kNSD bit is set 
   */
  Bool_t IsNSD() const { return (fTriggers & kNSD); } 
  /**
   * Check for Empty bunch crossing *
   * 
   * @return true if kEmpty bit is set 
   */
  Bool_t IsEmpty() const { return (fTriggers & kEmpty); } 
  /**
   * Check for A-side trigger *
   * 
   * @return true if kA bit is set 
   */
  Bool_t IsBeamEmpty() const { return (fTriggers & kA); } 
  /**
   * Check for B(arrel) trigger *
   * 
   * @return true if kB bit is set 
   */
  Bool_t IsBeamBeam() const { return (fTriggers & kB); } 
  /**
   * Check for C-side trigger *
   * 
   * @return true if kC bit is set 
   */
  Bool_t IsEmptyBeam() const { return (fTriggers & kC); }  
  /**
   * Check for Empty trigger *
   * 
   * @return true if kE bit is set 
   */
  Bool_t IsEmptyEmpty() const { return (fTriggers & kE); }
  /**
   * Check for pileup from SPD *
   * 
   * @return true if kPileUp bit is set 
   */
  Bool_t IsPileUp() const { return (fTriggers & kPileUp); }    
  /**
   * Check for true NSD from MC *
   * 
   * @return true if kMCNSD bit is set 
   */
  Bool_t IsMCNSD() const { return (fTriggers & kMCNSD); }    
  /**
   * Check for Offline MB triggered *
   * 
   * @return true if kOffline bit is set 
   */
  Bool_t IsOffline() const { return (fTriggers & kOffline); }
  /**
   * Check for At least one SPD cluster *
   * 
   * @return true if kNClusterGt0 bit is set 
   */
  Bool_t IsNClusterGt0() const { return (fTriggers & kNClusterGt0); }
  /**
   * Check for V0-AND trigger *
   * 
   * @return true if kV0AND bit is set 
   */
  Bool_t IsV0AND() const { return (fTriggers & kV0AND); } 
  /**
   * Check for Satellite event *
   * 
   * @return true if kSatellite bit is set 
   */
  Bool_t IsSatellite() const { return (fTriggers & kSatellite); }
  /**
   * Check for SPD outlier event *
   * 
   * @return true if kSPDOutlier bit is set 
   */
  Bool_t IsSPDOutlier() const { return (fTriggers & kSPDOutlier); }
  /**
   * Check for SPD pile-up *
   * 
   * @return true if kPileupSPD bit is set 
   */
  Bool_t IsPileupSPD() const { return (fTriggers & kPileupSPD); }
  /**
   * Check for Track pile-up *
   * 
   * @return true if kPileupTrack bit is set 
   */
  Bool_t IsPileupTrack() const { return (fTriggers & kPileupTrack); }
  /**
   * Check for Out of bunch pileup *
   * 
   * @return true if kPileupBC bit is set 
   */
  Bool_t IsPileupBC() const { return (fTriggers & kPileupBC); }
  /**
   * Check for SPD pile-up in mult bins *
   * 
   * @return true if kPileupBins bit is set 
   */
  Bool_t IsPileupBins() const { return (fTriggers & kPileupBins); }
  /* @} */
  /** 
   * @{
   * @name Check flags 
   */
  /** 
   * Check if the data has been secondary corrected by MC maps 
   * 
   * @return true if secondary corrected via MC maps
   */
  Bool_t IsSecondaryCorrected() const { return TestBit(kSecondary); }
  /** 
   * Check if vertex bias correction was applied 
   * 
   * @return true if MC based vertex bias was applied 
   */
  Bool_t IsVertexBiasCorrected() const { return TestBit(kVertexBias); }
  /** 
   * Check if acceptance correction (from dead strips) was applied 
   * 
   * @return true if acceptance correction was applied 
   */
  Bool_t IsAcceptanceCorrected() const { return TestBit(kAcceptance); }
  /** 
   * Check if merging efficiency (from MC) was applied 
   * 
   * @return true if merging efficiency was applied
   */
  Bool_t IsMergingEfficiencyCorrected() const { 
    return TestBit(kMergingEfficiency); }
  /** 
   * Check if an empirical correction was applied in the event level. 
   * 
   * @return True if the empirical correction was applied per event. 
   */
  Bool_t IsEmpiricalCorrected() const { return TestBit(kEmpirical); }
  /** 
   * Check if the output is the sum (not average) in regions of
   * overlap between detectors.
   * 
   * 
   * @return true if the sum (not average) is stored in overlap
   * regions.
   */
  Bool_t IsSumSignal() const { return TestBit(kSum); }
  /* @} */
  /** 
   * @{ 
   * @name Other services 
   */
  /** 
   * Get the name of the object 
   * 
   * @return Name of object 
   */
  const Char_t* GetName() const { return (fIsMC ? "ForwardMC" : "Forward"); }
  /** 
   * Clear all data 
   * 
   * @param option  Passed on to TH2::Reset verbatim
   */
  void Clear(Option_t* option="");
  /** 
   * browse this object 
   * 
   * @param b Browser 
   */
  void Browse(TBrowser* b);
  /** 
   * This is a folder 
   * 
   * @return Always true
   */
  Bool_t IsFolder() const { return kTRUE; } // Always true 
  
  /** 
   * Print content 
   * 
   * @param option Passed verbatim to TH2::Print 
   */
  void Print(Option_t* option="") const;
  /* @} */
  /** 
   * @{
   * @name Setters 
   */
  /* @} */
  /** 
   * @{ 
   * @name General tests 
   */
  /** 
   * Check if event meets the passses requirements.   
   *
   * It returns true if @e all of the following is true 
   *
   * - The trigger is within the bit mask passed.
   * - The vertex is within the specified limits. 
   * - The centrality is within the specified limits, or if lower
   *   limit is equal to or larger than the upper limit.
   * 
   * Note, for data with out a centrality estimate (e.g., pp), one
   * must pass equal centrality cuts, or no data will be accepted.  In
   * other words, for pp data, always pass cMin=0, cMax=0
   *
   * If a histogram is passed in the last parameter, then that
   * histogram is filled with the trigger bits. 
   * 
   * @param triggerMask  Trigger mask. All bits in this mask must be
   *                     set (AND)
   * @param vzMin        Minimum @f$ v_z@f$ (in centimeters)
   * @param vzMax        Maximum @f$ v_z@f$ (in centimeters) 
   * @param cMin         Minimum centrality (in percent)
   * @param cMax         Maximum centrality (in percent)
   * @param hist         Histogram to fill 
   * @param status       Histogram to fill 
   * @param filterMask   Mask of trigger bits to filter on. If any bit
   * in this mask is set, reject the event (OR)
   * 
   * @return @c true if the event meets the requirements 
   */
  Bool_t CheckEvent(UInt_t   triggerMask=kInel,
		    Double_t vzMin=-10, Double_t vzMax=10,
		    Double_t cMin=0,    Double_t cMax=100, 
		    TH1*     hist=0,
		    TH1*     status=0,
		    UInt_t   filterMask=kDefaultFilter) const;
  /** 
   * Fill the trigger histogram previously made with MakeTriggerHistogram
   * 
   * @param triggerMask  Mask 
   * @param trg Triggers
   * @param hist Histogram to fill
   */
  static void FillTriggerHistogram(UInt_t triggerMask, UInt_t trg, TH1* hist);
  /** 
   * Check wether this event was vetoed. 
   * 
   * @param vzMin         Least @f$\mbox{IP}_{z}@f$ 
   * @param vzMax         Largest  @f$\mbox{IP}_{z}@f$ 
   * @param hist          Trigger histogram 
   * @param status        Status histogram 
   * @param filterMask    Filter mask. Interpreted as an OR pattern of bits 
   *                      we should filter out. 
   * 
   * @return true if the event was not vetoed. 
   */
  Bool_t FilterEvent(Double_t vzMin, Double_t vzMax,
		     TH1* hist, TH1* status,
		     UInt_t filterMask) const;
  /** 
   * Make a histogram to record triggers in. 
   *
   * The bins defined by the trigger enumeration in this class.  One
   * can use this enumeration to retrieve the number of triggers for
   * each class.
   * 
   * @param name Name of the histogram 
   * @param mask Trigger mask 
   * 
   * @return Newly allocated histogram 
   */
  static TH1I* MakeTriggerHistogram(const char* name="triggers",
				    UInt_t mask=0);
  /** 
   * Make a histogram to record status in. 
   *
   * The bins defined by the status enumeration in this class.  
   * 
   * @param name Name of the histogram 
   * 
   * @return Newly allocated histogram 
   */
  static TH1I* MakeStatusHistogram(const char* name="status");
  /* @} */
protected: 
  /** From MC or not */
  Bool_t   fIsMC;       // Whether this is from MC 
  /** Histogram of @f$ N_{ch}(\eta,\phi)@f$ for this event */
  TH2D     fHist;       // Histogram of N_{ch}(eta,phi)
  /** Trigger bits */
  UInt_t   fTriggers;   // Trigger bit mask 
  /** Interaction point @f$ z@f$ coordinate */
  Float_t  fIpZ;        // Z coordinate of the interaction point
  /** Centrality */
  Float_t  fCentrality; // Event centrality 
  /** Number of clusters in @f$|\eta|<1@f$ */
  UShort_t fNClusters;  // Number of SPD clusters in |eta|<1
  /** Invalid value for interaction point @f$ z@f$ coordinate */
  static const Float_t fgkInvalidIpZ; // Invalid IpZ value 
  ClassDef(AliAODForwardMult,7); // AOD forward multiplicity 
};

//____________________________________________________________________
inline Bool_t
AliAODForwardMult::InRange(Float_t low, Float_t high) const 
{
  return HasIpZ() && fIpZ >= low && fIpZ < high;
}

//____________________________________________________________________
inline Bool_t 
AliAODForwardMult::IsTriggerBits(UInt_t bits) const 
{
  return IsTriggerBits(bits, fTriggers);
}
//____________________________________________________________________
inline Bool_t 
AliAODForwardMult::IsTriggerBits(UInt_t bits, UInt_t trig)
{
  const UInt_t mask = ~kInclusive;
  if (bits & kInclusive)
    return trig > 0 && ((trig & mask & bits) != 0);
  return trig > 0 && ((trig & mask & bits) == bits); 
}

//____________________________________________________________________
inline Bool_t 
AliAODForwardMult::IsTriggerOrBits(UInt_t bits) const 
{ 
  return HasTrigger() && ((fTriggers & bits) != 0);
}

#endif
// Local Variables:
//  mode: C++
// End:

