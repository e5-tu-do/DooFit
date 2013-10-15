// $Id: $
#ifndef MULTIHISTENTRY_H 
#define MULTIHISTENTRY_H 1

// Include files
#include <iostream>

#include <TObject.h>

#include <RooCategoryProxy.h>
#include <RooRealProxy.h>

#include <P2VV/RooMultiEffResModel.h>

/** @class MultiHistEntry MultiHistEntry.h
 *  
 *
 *  @author Roel Aaij
 *  @date   2012-08-21
 */
class MultiHistEntry {
public:

   MultiHistEntry()
      : m_rawEff(0), m_rawRel(0), m_efficiency(0), m_relative(0),
        m_index(0)
   {
   }

   MultiHistEntry(const std::map<RooAbsCategory*, std::string>& categories,
                  RooAbsEffResModel* efficiency, RooAbsReal* relative)
      : m_rawCats(categories), m_rawEff(efficiency), m_rawRel(relative),
        m_efficiency(0), m_relative(0), m_index(0)
   {
   }

   MultiHistEntry(const MultiHistEntry& other)
      : m_rawCats(other.m_rawCats), m_rawEff(other.m_rawEff), m_rawRel(other.m_rawRel),
        m_index(other.m_index)
   {
      for (std::map<RooCategoryProxy*, std::string>::const_iterator it = other.m_categories.begin(),
              end = other.m_categories.end(); it != end; ++it) {
         RooCategoryProxy* proxy = new RooCategoryProxy(*it->first);
         m_categories.insert(make_pair(proxy, it->second));
      }
      
      if (other.m_efficiency) {
         m_efficiency = new RooRealProxy(*other.m_efficiency);
      } else {
         m_efficiency = 0;
      }
      if (other.m_relative) {
         m_relative = new RooRealProxy(*other.m_relative);
      } else {
         m_relative = 0;
      }
   }

   MultiHistEntry(const MultiHistEntry& other, RooAbsEffResModel* efficiency)
      : m_rawCats(other.m_rawCats), m_rawEff(efficiency), m_rawRel(other.m_rawRel),
        m_efficiency(0), m_index(other.m_index)
   {
      for (std::map<RooCategoryProxy*, std::string>::const_iterator it = other.m_categories.begin(),
              end = other.m_categories.end(); it != end; ++it) {
         RooCategoryProxy* proxy = new RooCategoryProxy(*it->first);
         m_categories.insert(make_pair(proxy, it->second));
      }
      if (other.m_relative) {
         m_relative = new RooRealProxy(*other.m_relative);
      } else {
         m_relative = 0;
      }
   }

   MultiHistEntry(const MultiHistEntry& other, RooMultiEffResModel* parent)
      : m_rawCats(other.m_rawCats), m_rawEff(other.m_rawEff), m_rawRel(other.m_rawRel),
        m_index(other.m_index)
   {
      std::string name;
      if (other.m_efficiency) {
         m_efficiency = new RooRealProxy(other.m_efficiency->GetName(), parent, *other.m_efficiency);
      } else {
         RooResolutionModel* effModel = dynamic_cast<RooResolutionModel*>(m_rawEff);
         name = effModel->GetName(); name += "_proxy";
         m_efficiency = new RooRealProxy(name.c_str(), name.c_str(), parent, *effModel);
      }
      m_rawEff = 0;
         
      if (other.m_relative) {
         m_relative = new RooRealProxy(other.m_relative->GetName(), parent, *other.m_relative);
      } else {
         RooResolutionModel* effModel = dynamic_cast<RooResolutionModel*>(m_rawEff);
         name = effModel->GetName(); name += "_proxy";
         m_relative = new RooRealProxy(name.c_str(), name.c_str(), parent, *effModel);
      }
      m_rawRel = 0;
      
      if (!other.m_categories.empty()) {
         for (std::map<RooCategoryProxy*, std::string>::const_iterator it = other.m_categories.begin(),
                 end = other.m_categories.end(); it != end; ++it) {
            RooCategoryProxy* proxy = new RooCategoryProxy(it->first->GetName(), parent, *(it->first));
            m_categories.insert(make_pair(proxy, it->second));
         }
      } else {
         for (std::map<RooAbsCategory*, std::string>::const_iterator it = other.m_rawCats.begin(),
                 end = other.m_rawCats.end(); it != end; ++it) {
            name = it->first->GetName(); name += "_proxy";
            RooCategoryProxy* proxy = new RooCategoryProxy(name.c_str(), name.c_str(), parent,
                                                           *(it->first));
            m_categories.insert(make_pair(proxy, it->second));
         }
      }
      m_rawCats.clear();
   }

   virtual ~MultiHistEntry()
   {
      for (std::map<RooCategoryProxy*, std::string>::const_iterator it = m_categories.begin(),
              end = m_categories.end(); it != end; ++it) {
         if (it->first) delete it->first;
      }
      m_categories.clear();
      if (m_efficiency) delete m_efficiency;
      if (m_relative) delete m_relative;
   }

   RooResolutionModel* effModel() {
      RooResolutionModel* model = 0;
      if (m_efficiency) {
         model = dynamic_cast<RooResolutionModel*>(m_efficiency->absArg());
      } else {
         model = dynamic_cast<RooResolutionModel*>(m_rawEff);
      }
      // if (!model) {
      //    const RooAbsArg& tmp = m_efficiency ? m_efficiency->arg() : dynamic_cast<const RooAbsArg&>(*m_rawEff);
      //    cout << tmp.GetName() << " is not a " << endl;
      //    assert(false);
      // }
      return model;
   }

   const RooResolutionModel* effModel() const {
      return const_cast<MultiHistEntry*>(this)->effModel();
      // }
   }

   RooAbsEffResModel* efficiency() {
      RooAbsEffResModel* eff = 0;
      if (m_efficiency) {
         eff = dynamic_cast<RooAbsEffResModel*>(m_efficiency->absArg());
      } else {
         eff = m_rawEff;
      }
      assert(eff);
      return eff;
   }

   const RooAbsEffResModel* efficiency() const {
      return const_cast<MultiHistEntry*>(this)->efficiency();
      // }
   }

   void setEfficiency(RooAbsEffResModel* eff) {
      if (m_efficiency) {
         RooResolutionModel* effModel = dynamic_cast<RooResolutionModel*>(eff);
         m_efficiency->setArg(*effModel);
      } else {
         m_rawEff = eff;
      }
   }

   void setRelative(RooAbsReal* rel) {
      if (m_relative) {
         m_relative->setArg(*rel);
      } else {
         m_rawRel = rel;
      }
   }
   
   RooAbsReal* relative() {
      return m_relative ? dynamic_cast<RooAbsReal*>(m_relative->absArg()) : m_rawRel;
   }

   const RooAbsReal* relative() const {
      return const_cast<MultiHistEntry*>(this)->relative();      
   }

   void setParent(RooMultiEffResModel* parent) {
      std::string name;
      if (m_efficiency != 0) {
         RooRealProxy* temp = new RooRealProxy(m_efficiency->GetName(), parent, *m_efficiency);
         delete m_efficiency;
         m_efficiency = temp;
      } else {
         RooResolutionModel* effModel = dynamic_cast<RooResolutionModel*>(m_rawEff);
         name = effModel->GetName(); name += "_proxy";
         m_efficiency = new RooRealProxy(name.c_str(), name.c_str(), parent, *effModel);
      }
      m_rawEff = 0;

      if (m_relative != 0) {
         RooRealProxy* temp = new RooRealProxy(m_relative->GetName(), parent, *m_relative);
         delete m_relative;
         m_relative = temp;
      } else {
         name = m_rawRel->GetName(); name += "_proxy";
         m_relative = new RooRealProxy(name.c_str(), name.c_str(), parent, *m_rawRel);
      }
      m_rawRel = 0;
      
      if (!m_categories.empty()) {
         std::map<RooCategoryProxy*, std::string> temp;
         for (std::map<RooCategoryProxy*, std::string>::const_iterator it = m_categories.begin(),
                 end = m_categories.end(); it != end; ++it) {
            RooCategoryProxy* proxy = new RooCategoryProxy(it->first->GetName(), parent, *(it->first));
            delete it->first;
            temp.insert(make_pair(proxy, it->second));
         }
         m_categories = temp;
      } else {
         for (std::map<RooAbsCategory*, std::string>::const_iterator it = m_rawCats.begin(),
                 end = m_rawCats.end(); it != end; ++it) {
            name = it->first->GetName(); name += "_proxy";
            RooCategoryProxy* proxy = new RooCategoryProxy(name.c_str(), name.c_str(), parent,
                                                           *(it->first));
            m_categories.insert(make_pair(proxy, it->second));
         }
      }
      m_rawCats.clear();      
   }

   void addCategories(RooArgSet& r) const {
      if (!m_rawCats.empty()) {
         for (std::map<RooAbsCategory*, std::string>::const_iterator it = m_rawCats.begin(),
                 end = m_rawCats.end(); it != end; ++it) {
            if (!it->first) continue;
            r.add(*(it->first));
         }
      } else {
         for (std::map<RooCategoryProxy*, std::string>::const_iterator it = m_categories.begin(),
                 end = m_categories.end(); it != end; ++it) {
            if (!it->first) continue;
            r.add(it->first->arg());
         }
      }
   }

   bool thisEntry() const {
      bool r = true;
      for (std::map<RooCategoryProxy*, std::string>::const_iterator it = m_categories.begin(),
              end = m_categories.end(); it != end; ++it) {
         const RooAbsCategory* cat = static_cast<const RooAbsCategory*>(it->first->absArg());
         if (strcmp(cat->getLabel(), it->second.c_str()) != 0) {
            r = false;
            break;
         }
      }
      return r;
   }

   void setIndex(const Int_t index) {
      m_index = index;
   }
   
   Int_t index() const {
      return m_index;
   }

   void select()
   {
      if (!m_rawCats.empty()) {
         for (std::map<RooAbsCategory*, std::string>::const_iterator it = m_rawCats.begin(),
                 end = m_rawCats.end(); it != end; ++it) {
            if (!it->first) continue;
            RooAbsCategoryLValue* lval = dynamic_cast<RooAbsCategoryLValue*>(it->first);
            lval->setLabel(it->second.c_str());
         }
      } else {
         for (std::map<RooCategoryProxy*, std::string>::const_iterator it = m_categories.begin(),
                 end = m_categories.end(); it != end; ++it) {
            RooAbsCategoryLValue* lval = dynamic_cast<RooAbsCategoryLValue*>(it->first->absArg());
            lval->setLabel(it->second.c_str());
         }
      }
   }

   void print()
   {
      for (std::map<RooAbsCategory*, std::string>::const_iterator it = m_rawCats.begin(),
              end = m_rawCats.end(); it != end; ++it) {
         if (!it->first) continue;
         it->first->Print();
         std::cout << it->second << std::endl;
      }
      if (m_rawEff) {
         RooResolutionModel* effModel = dynamic_cast<RooResolutionModel*>(m_rawEff);
         effModel->Print();
      } else {
         std::cout << "m_rawEff == 0" << std::endl;
      }
      if (m_rawRel) {
         m_rawRel->Print();
      } else {
         std::cout << "m_rawRel == 0" << std::endl;
      }
      for (std::map<RooCategoryProxy*, std::string>::const_iterator it = m_categories.begin(),
              end = m_categories.end(); it != end; ++it) {
         if (!it->first) continue;
         it->first->Print();
         std::cout << it->second << std::endl;
      }
      if (m_efficiency) {
         m_efficiency->Print();
      } else {
         std::cout << "m_efficiency == 0" << std::endl;
      }
      if (m_relative) {
         m_relative->Print();
      } else {
         std::cout << "m_relative == 0" << std::endl;
      }      
   }

private:


   std::map<RooAbsCategory*, std::string> m_rawCats;
   RooAbsEffResModel* m_rawEff; //!
   RooAbsReal* m_rawRel; //!

   std::map<RooCategoryProxy*, std::string> m_categories;
   RooRealProxy* m_efficiency;
   RooRealProxy* m_relative;
   Int_t m_index;

};
#endif // MULTIHISTENTRY_H
