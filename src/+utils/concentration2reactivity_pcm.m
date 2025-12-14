function p = concentration2reactivity_pcm(c, data)
    p = 1e5*(data.absorptionSection.Xe135/(data.neutronsPerFission*data.macroFissionSection))*c;
end