#ifndef _VERTICAL_LIST_SERVICE_PROVIDER_
#define _VERTICAL_LIST_SERVICE_PROVIDER_

class screen_generator; // forward reference

class vertical_list_service_provider : public window_service_provider
{
    public:
        vertical_list_service_provider();

        virtual int GetType() {return GX_TYPE_VERTICAL_LIST;}
        virtual CString GetTypeName(widget_info *) {return CString("GX_TYPE_VERTICAL_LIST");}
        virtual CString GetShortName() {return CString("vertical_list");}
        virtual CString GetVarDeclaration();
        virtual CString GetControlBlockName() {return CString("GX_VERTICAL_LIST");}

        virtual CString DeclarePropertiesStruct();
        virtual CString WriteExtendedProperties(screen_generator *gen, CString &scope, widget_info *info);
        virtual CString GetCreateFromDefFunctionName();
        virtual CString GetCreateFromDefFunction(int version);

        virtual widget_info *CreateNewInstance(GX_WIDGET *parent);
        virtual GX_WIDGET *GenerateFromInfo(GX_WIDGET *parent, widget_info *info);
        
        virtual void SaveToProject(xml_writer &, studiox_project *project, int display, widget_info *);
        virtual void ReadFromProject(xml_reader &, studiox_project *project, int display, widget_info *info, ULONG valid_styles);
};

#endif
