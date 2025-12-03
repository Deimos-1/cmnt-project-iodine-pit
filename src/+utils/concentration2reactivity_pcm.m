function p = concentration2reactivity_pcm(c, data)
    p = 1e5*(data.absobtionSection.Xe135/(2.3*data.macroFissionSection))*c ;
end