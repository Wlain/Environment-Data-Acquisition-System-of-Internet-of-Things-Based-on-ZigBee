package com.springboot.controller;

import com.springboot.entities.Gas;
import com.springboot.entities.Guangzhao;
import com.springboot.service.GasService;
import com.springboot.service.GuangzhaoService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by 10497 on 2017/6/12.
 */
@RestController
public class GasController {
    @Autowired
    private GasService gasService;
    @RequestMapping("findGas")
    public Map<String,Object> findAllGas(){
        Map<String,Object> gzMap = new HashMap<String,Object>();
        List<Gas> list = gasService.findAllGas();
        gzMap.put("data",list);
        return gzMap;
    }
    @RequestMapping("Gas")
    public Map<String,Object> addGas(Integer gs){
        Map<String, Object> gzMap = new HashMap<String, Object>();
        System.out.println(gs);
        if(gs==null){
            gzMap.put("cause","可燃气体值不能为空");
            return gzMap;
        }
        try {
           gasService.addGas(gs);
        } catch (Exception e) {
            gzMap.put("status", "fail");
            return gzMap;
        }
        gzMap.put("status", "success");
        return gzMap;
    }

}
