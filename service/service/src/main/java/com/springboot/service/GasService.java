package com.springboot.service;

import com.springboot.entities.Gas;
import com.springboot.repositroy.GasRepositoty;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import javax.transaction.Transactional;
import java.util.Date;
import java.util.List;

/**
 * Created by 10497 on 2017/6/12.
 */
@Service
@Transactional
public class GasService {
    @Autowired
    private GasRepositoty gasRepositoty;
    public List<Gas> findAllGas(){
        List<Gas> list = gasRepositoty.findAllGas();
        return  list;
    }
    public  void addGas(int g){
        Gas gas=new Gas();
        //guangzhao.setGz(gz);
       // guangzhao.setTime(new Date());
        //guangzhaoRepositoty.saveAndFlush(guangzhao);
        gas.setGas(g);
        gas.setDate(new Date());
        gasRepositoty.saveAndFlush(gas);
    }
}
